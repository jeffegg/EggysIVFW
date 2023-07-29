import traceback

import serial
import binascii
import time
from importlib import reload
import intel_hex_reader
import serial.tools.list_ports

reload(intel_hex_reader)

class FWUpdater:
    def __init__(self):
        self._fw_update_filename = "test.hex"
        self._full_hex_file = None
        self._start_of_frame = bytearray([0xFF, 0x0, 0xFF])
        self._start_of_header = bytearray([0xA5, 0x3F])
        self._src = 0xF # Send as broadcast, since using 0x10 causes address assignment to revert
        self._dest = 0xC
        self._cmd = 0xF5
        self._reset_cmd = bytearray([0xFF, 0x0, 0xFF, 0xA5, 0x3F, 0xC, 0xF, 0xF7, 0x1, 0x1])
        self._rs485_loc = "/dev/ttyUSB1"
        self.found_valves = {}
        self._start_address = 0xA0
        self._current_unassigned_address = self._start_address

    @property
    def fw_update_filename(self):
        return self._fw_update_filename

    @fw_update_filename.setter
    def fw_update_filename(self, value):
        self._fw_update_filename = value

    def _calculate_and_append_checksum(self, bytes_to_check: bytearray):
        if bytes_to_check[0:3] != self._start_of_frame:
            raise BufferError("This doesn't seem to be a valid packet")
        check_sum = 0
        for i in range (3, len(bytes_to_check)):
            check_sum += bytes_to_check[i]
        check_sum = check_sum & 0xFFFF
        bytes_to_check.append((check_sum >> 0x8) & 0xFF)
        bytes_to_check.append((check_sum >> 0x0) & 0xFF)

    def reset_valve(self, valve_id: bytearray):
        address = self.found_valves[valve_id]
        new_reset_cmd = self._reset_cmd.copy()
        new_reset_cmd[5] = address # Fix up address
        self._calculate_and_append_checksum(new_reset_cmd)
        print("Resetting valve 0x" + valve_id.hex() + " at address " + hex(address))
        ser = serial.Serial(self._rs485_loc, 9600)
        ser.write(new_reset_cmd)
        ser.close()

    def setup(self, rs485_loc="/dev/ttyUSB1"):
        comport_list = serial.tools.list_ports.comports()

        for comport in comport_list:
            print(comport.device)

        self._rs485_loc = rs485_loc
        print("Enumerating Valves")
        self.enumerate_valves()
        self.setup_addresses()
        time.sleep(1)
        self.verify_valve_addresses()
        for valve_id in self.found_valves.keys():
            time.sleep(1)
            self.reset_valve(valve_id)
        time.sleep(1)
        #print("sleeping")
        #input("Press Enter to continue...")
        self.setup_addresses()

    def goto_fw_update_mode(self):
        #self.setup()
        self.setup_addresses()
        time.sleep(1)
        ser = serial.Serial(self._rs485_loc, 9600)

        for valve_id in self.found_valves.keys():
            ihr = intel_hex_reader.intel_hex_reader(self._fw_update_filename)
            ihr.open()
            ihr.convert_to_64_byte_lines()
            for line in ihr._parse_64_byte_list:
                address = self.found_valves[valve_id]
                fw_packet = bytearray([0xFF, 0x0, 0xFF, 0xA5, 0x3F, 0xC, 0xF, 0xF5, 0x2, 0x1, 0x2])
                fw_packet[5] = address
                print("programming 0x" + line["address"])
                data = line["address"] + "01" + line["data"]
                print(type(line["address"] ))
                print(line["address"] )
                print(data)
                fw_packet[8] = 2 + 1 + 2 + line["byte_count"]
                fw_packet += bytearray.fromhex(data)
                self._calculate_and_append_checksum(fw_packet)
                ser.write(fw_packet)
                time.sleep(1)
        ser.close()

    def exit_fw_update_mode(self):
        self.cause_valve_address_revert()
        time.sleep(1)
        ser = serial.Serial(self._rs485_loc, 9600)
        fw_packet = bytearray([0xFF, 0x0, 0xFF, 0xA5, 0x3F, 0xC, 0xF, 0xF5, 0x2, 0x0, 0x0])
        self._calculate_and_append_checksum(fw_packet)
        ser.write(fw_packet)
        ser.close()

    def enumerate_valves(self, enumerate_timeout=10):
        self._enumerate_valves(enumerate_timeout)
        if len(self.found_valves) == 0:
            print("No valves found, trying to revert valve addresses")
            self.cause_valve_address_revert()
            print("Sleeping for 20 seconds to allow valves to start sending hails again")
            time.sleep(20)
            self._enumerate_valves(enumerate_timeout)
        print("Found the following valves:")
        for valve_id in self.found_valves.keys():
            print(valve_id)
            print('\t0x' + str(valve_id.hex()))

    def _enumerate_valves(self, enumerate_timeout):
        ser = serial.Serial(self._rs485_loc, 9600)
        ser.timeout = enumerate_timeout
        print("Listening to " + self._rs485_loc + " for " + str(enumerate_timeout) + " seconds.")
        last_time = time.time()
        elapsed_time = 0
        while elapsed_time < enumerate_timeout:
            ser.flushInput()
            try:
                valve = ser.read(19)
                new_valve_id = self.decode_unique_id(valve)
                if new_valve_id not in self.found_valves.keys():
                    self.found_valves[new_valve_id] = None

            except BufferError:
                pass
            elapsed_time = time.time() - last_time
        ser.close()

    def cause_valve_address_revert(self):
        ser = serial.Serial(self._rs485_loc, 9600)
        address_reset = bytearray([0xFF, 0x0, 0xFF, 0xA5, 0x3F, 0xC, 0x10, 0x51, 0x1, 0x1])
        self._calculate_and_append_checksum(address_reset)
        ser.write(address_reset)
        ser.close()



    def setup_addresses(self):
        ser = serial.Serial(self._rs485_loc, 9600)
        print("Setting valve addresses")
        print("Address  Valve ID")

        for valve_id in self.found_valves.keys():
            if self.found_valves[valve_id] is None:
                address_to_setup = self._current_unassigned_address
                self._current_unassigned_address += 1
            else:
                address_to_setup = self.found_valves[valve_id]
            change_address_packet = self._start_of_frame.copy()
            change_address_packet = change_address_packet + self._start_of_header.copy()
            change_address_packet.append(self._dest)
            change_address_packet.append(self._src)
            change_address_packet.append(0x50)
            change_address_packet.append(8)
            change_address_packet.append(address_to_setup)
            change_address_packet.append(0x1)
            change_address_packet += valve_id #.to_bytes(6, 'big')
            self._calculate_and_append_checksum(change_address_packet)
            ser.write(change_address_packet)
            #print(ser.read(12))
            print(hex(address_to_setup) + "     0x" + str(valve_id.hex()))
            self.found_valves[valve_id] = address_to_setup
        ser.close()

    def verify_valve_addresses(self):
        print("Verifying valve addresses")
        print("Address  Valve ID       DID    RID")
        ser = serial.Serial(self._rs485_loc, 9600)
        for valve_id in self.found_valves.keys():
            address = self.found_valves[valve_id]
            change_address_packet = self._start_of_frame.copy()
            change_address_packet = change_address_packet + self._start_of_header.copy()
            change_address_packet.append(address)
            change_address_packet.append(self._src)
            change_address_packet.append(0xF0)
            change_address_packet.append(1)
            change_address_packet.append(0)
            self._calculate_and_append_checksum(change_address_packet)
            ser.write(change_address_packet)
            valve_info = ser.read(29)

            read_id = self.decode_unique_id(valve_info)
            print(valve_info)
            did, rid = self.decode_did_rid(valve_info)
            print(hex(valve_info[6]) + "     0x" + str(read_id.hex()) + " 0x" + str(did.hex()) + " 0x" + str(rid.hex()))
        ser.close()

    def decode_unique_id(self, valve_packet: bytearray) -> bytearray:
        if valve_packet[0:3] != self._start_of_frame:
            raise BufferError("This doesn't seem to be a valid packet")

        if (valve_packet[7] == 0x52) or (valve_packet[7] == 0xF1):
            if valve_packet[10] != 0x80:
                raise BufferError("This doesn't seem to be a valid packet for serial numbers, etc, byte 10 should be 0x80. Instead was " + hex(valve_packet[10]))
            unique_id = valve_packet[11:17]
            return unique_id

    def decode_valve_hail(self, valve_packet: bytearray) -> dict:
        if valve_packet[0:3] != self._start_of_frame:
            raise BufferError("This doesn't seem to be a valid packet")
        if valve_packet[7] == 0x52:
            packet_lenght = valve_packet[8]
            address = hex(valve_packet[6])
            valve_packet = valve_packet[9:]
            unique_id = valve_packet[0:6].hex('-')
            fw_version = valve_packet[6:10].hex()
            fw_date = valve_packet[10:14].hex()
            did = valve_packet[14:16].hex()
            rid = valve_packet[16:18].hex()
            fw_branch_size = int(valve_packet[18:19].hex(), 16)
            fw_tag_size = int(valve_packet[19:20].hex(), 16)
            fw_branch = valve_packet[20:20+fw_branch_size].decode("ascii")
            fw_tag = valve_packet[20 + fw_branch_size:20 + fw_branch_size + fw_tag_size].decode("ascii")

            return {"Type": "Eggy's FW", "Address": address, "UUID": unique_id, "Git Hash": fw_version, "Git Date": fw_date, "DID": did, "RID": rid, "Branch": fw_branch, "Tag": fw_tag}

        return None

    def id_new_values(self, enumerate_timeout=60):
        ser = serial.Serial(self._rs485_loc, 9600)
        id_valve_packet = bytearray([0xFF, 0x0, 0xFF, 0xA5, 0x3F, 0xF, 0xF, 0x12, 0x1, 0x1])
        self._calculate_and_append_checksum(id_valve_packet)
        ser.write(id_valve_packet)
        ser.close()
        ser = serial.Serial(self._rs485_loc, 9600)
        ser.timeout = enumerate_timeout
        print("Listening to " + self._rs485_loc + " for " + str(enumerate_timeout) + " seconds.")
        last_time = time.time()
        elapsed_time = 0
        while elapsed_time < enumerate_timeout:
            ser.flushInput()
            try:
                valve:bytearray = ser.read(10000)

                id_packets = []
                for i in range(len(valve) - 3):
                    if valve[i+0:i+3] == self._start_of_frame:
                        packet_len = int(valve[i+8:i+9].hex(), 16)
                        id_packets.append(valve[i : i+packet_len+9+2])

                for entry in id_packets:
                    try:
                        new_valve_id = {}
                        new_valve_id = self.decode_valve_hail(entry)
                    except Exception:
                        try:
                            read_id = self.decode_unique_id(entry)
                            print({"Type": "Pentairs's FW", "Address": hex(entry[6]), "UUID": read_id.hex('-')})
                        except:
                            continue
                    else:
                        print(new_valve_id)

            except BufferError:
                pass
            elapsed_time = time.time() - last_time
        ser.close()

    def decode_did_rid(self, valve_packet: bytearray) -> (bytearray, bytearray):
        if valve_packet[0:3] != self._start_of_frame:
            raise BufferError("This doesn't seem to be a valid packet")

        if valve_packet[7] == 0xF1:
            if valve_packet[10] != 0x80:
                raise BufferError(
                    "This doesn't seem to be a valid packet for serial numbers, etc, byte 10 should be 0x80. Instead was 0x" + hex(valve_packet[10]))

            if valve_packet[17:21] != bytearray([0x1, 0x2, 0xFF, 0xFF]):
                raise BufferError(
                    "This doesn't seem to be a valid packet for serial numbers, etc, byte 17-20 should be 0x0102ffff. Instead was 0x" + valve_packet[17:21].hex())
            if valve_packet[25:27] != bytearray([0x0, 0x40]):
                raise BufferError(
                    "This doesn't seem to be a valid packet for serial numbers, etc, byte 17-20 should be 0x0040. Instead was 0x" + valve_packet[25:26].hex())
            did = valve_packet[21:23]
            rid = valve_packet[23:25]
            return did, rid

        else:
            raise BufferError(f"valve_packet[7]: {str(valve_packet[7])}")
