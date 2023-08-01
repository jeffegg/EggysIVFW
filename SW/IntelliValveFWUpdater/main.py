import traceback
import requests
import serial
import binascii
import time
from importlib import reload
import intel_hex_reader
import serial.tools.list_ports
from tabulate import tabulate
import readline
import tempfile
import os

PENTAIR_FW_TYPE = "Pentairs's FW"
EGGY_FW_TYPE = "Eggy's FW"
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

            return {"Type": EGGY_FW_TYPE, "Address": address, "UUID": unique_id, "Git Hash": fw_version, "Git Date": fw_date, "DID": did, "RID": rid, "Branch": fw_branch, "Tag": fw_tag}

        return None

    def fw_updater(self, alt_fw_path="", rs485_port="", skip_valve_enum=False, enumerate_timeout=30, valve_uuid="", valve_type="", valve_address=0x0, num_retries=10):
        if rs485_port != "":
            self._rs485_loc = rs485_port
        else:
            self._rs485_loc = self.select_com_port()
        device_list = []
        if not skip_valve_enum:
            device_list = self.find_all_valves(enumerate_timeout=enumerate_timeout)
        if alt_fw_path != "":
            self._fw_update_filename = alt_fw_path
        else:
            self._fw_update_filename = self.list_firmware(get_fw=True)
            if self._fw_update_filename == "":
                print("No FW selected, downloading latest")
                self._fw_update_filename = self.get_latest_firmware()
                if self._fw_update_filename == "":
                    print("FW was not downloaded, please manually download")
                    return
        if (valve_uuid != "") and (valve_address != 0x0) and (valve_type != ""):
            valve_uuid_selected = bytearray.fromhex(valve_uuid.replace("-", ""))
            valve_type = valve_type
            valve_address = valve_address
        else:
            valve_uuid_selected, valve_type, valve_address = self.select_valve_for_update(device_list)
        if valve_uuid_selected is None:
            print("No valve selected, exiting")
            return
        address_list = []
        duplicate_address_list = []
        for device in device_list:
            addr = int(device['Address'], 16)
            if addr in address_list:
                duplicate_address_list.append(addr)
            address_list.append(addr)

        for device in device_list:
            addr = int(device['Address'], 16)
            if (addr == 0xC) or (addr in duplicate_address_list):
                new_address = 0xA0
                while new_address < 0xF8:
                    if new_address not in address_list:
                        break
                    new_address += 1
                if new_address == 0xF8:
                    print("Could not allocate an address from 0xA0-0xF8; exiting")
                device['Address'] = hex(new_address)
                address_list.append(new_address)
        print("Reallocated Valve Address if needed")
        print(tabulate(device_list, headers='keys'))

        self.quiet_all_valves(device_list, valve_uuid_selected)
        time.sleep(1)
        self.quiet_all_valves(device_list, valve_uuid_selected)
        time.sleep(1)
        self.quiet_all_valves(device_list, valve_uuid_selected)

        if valve_type == EGGY_FW_TYPE:
            self.goto_fw_update_mode_eggy(valve_uuid_selected, valve_address)
        else:
            self.reset_valve_new(valve_uuid_selected, valve_address)

        self.setup_addresses_new(valve_uuid_selected, valve_address)
        time.sleep(1)
        self.setup_addresses_new(valve_uuid_selected, valve_address)
        time.sleep(1)

        self.update_fw_eggy(valve_uuid_selected, valve_address, device_list, num_retries)

        print("Trying to force exit FW mode, sleeping for 20 seconds")
        time.sleep(20)
        self.exit_fw_update_mode_new(valve_address)
        self.exit_fw_update_mode_new(valve_address)
        print("Sleeping another 5 seconds")
        time.sleep(5)
        self.exit_fw_update_mode()
        self.exit_fw_update_mode()

    def quiet_all_valves(self, device_list, valve_uuid_selected):
        for device in device_list:
            if device['UUID'] != valve_uuid_selected.hex("-"):
                addr = int(device['Address'], 16)
                self.quiet_valve(device, addr)

    def quiet_valve(self, device:dict, new_address):
        if device["Type"] == PENTAIR_FW_TYPE:
            self.setup_addresses_new(bytearray.fromhex(device["UUID"].replace("-", "")), new_address)
            time.sleep(1)
            self.setup_addresses_new(bytearray.fromhex(device["UUID"].replace("-", "")), new_address)
            time.sleep(1)
        else:
            ser = serial.Serial(self._rs485_loc, 9600)
            fw_packet = bytearray([0xFF, 0x0, 0xFF, 0xA5, 0x3F, new_address, 0xF, 0x10, 0x6])
            fw_packet += bytearray.fromhex(device["UUID"].replace("-", ""))
            self._calculate_and_append_checksum(fw_packet)
            ser.write(fw_packet)
            ser.close()
            time.sleep(1)
            ser = serial.Serial(self._rs485_loc, 9600)
            fw_packet = bytearray([0xFF, 0x0, 0xFF, 0xA5, 0x3F, new_address, 0xF, 0x10, 0x6])
            fw_packet += bytearray.fromhex(device["UUID"].replace("-", ""))
            self._calculate_and_append_checksum(fw_packet)
            ser.write(fw_packet)
            ser.close()

    def exit_fw_update_mode_new(self, valve_address: int):
        self.cause_valve_address_revert()
        time.sleep(1)
        ser = serial.Serial(self._rs485_loc, 9600)
        fw_packet = bytearray([0xFF, 0x0, 0xFF, 0xA5, 0x3F, valve_address, 0xF, 0xF5, 0x2, 0x0, 0x0])
        self._calculate_and_append_checksum(fw_packet)
        ser.write(fw_packet)
        ser.close()

    def reset_valve_new(self, valve_uuid_selected: bytearray, valve_address: int):
        new_reset_cmd = self._reset_cmd.copy()
        new_reset_cmd[5] = valve_address # Fix up address
        self._calculate_and_append_checksum(new_reset_cmd)
        print(f'Forcing valve address: {hex(valve_address)} uuid: {valve_uuid_selected.hex("-")} to reset')
        ser = serial.Serial(self._rs485_loc, 9600)
        ser.write(new_reset_cmd)
        ser.close()

    def setup_addresses_new(self, valve_uuid_selected: bytearray, valve_address: int):
        ser = serial.Serial(self._rs485_loc, 9600)
        print(f'Setting valve {valve_uuid_selected.hex("-")} to {hex(valve_address)}')

        change_address_packet = self._start_of_frame.copy()
        change_address_packet = change_address_packet + self._start_of_header.copy()
        change_address_packet.append(self._dest)
        change_address_packet.append(self._src)
        change_address_packet.append(0x50)
        change_address_packet.append(8)
        change_address_packet.append(valve_address)
        change_address_packet.append(0x1)
        change_address_packet += valve_uuid_selected
        self._calculate_and_append_checksum(change_address_packet)
        ser.write(change_address_packet)

        ser.close()

    def update_fw_eggy(self, valve_uuid_selected: bytearray, valve_address: int, device_list, num_retries: int = 10):
        print(f"Opening serial port: {self._rs485_loc}")
        ser = serial.Serial(self._rs485_loc, 9600)
        ihr = intel_hex_reader.intel_hex_reader(self._fw_update_filename)
        ihr.open()
        ihr.convert_to_64_byte_lines()
        ser.timeout = 1
        expected_packet = bytes([0xFF, 0x0, 0xFF, 0xA5, 0x1, 0x10, valve_address, 0x1, 0x1, 0xF5])
        lenght_of_update = len(ihr._parse_64_byte_list)
        retry_of_retry = 0
        for line in ihr._parse_64_byte_list:
            read_packet = self.send_programming_info(line, ser, valve_address)
            try:
                if expected_packet != read_packet[0:10]:
                    print(f"Unexpected packet read: {read_packet[0:10].hex('-')} vs {expected_packet.hex('-')} on line {line}")
                    retry = 0
                    packet_sent = False
                    while retry < num_retries:
                        print(f"retry {retry} of {num_retries} and retry of retry {retry_of_retry}")
                        ser.close()
                        self.setup_addresses_new(valve_uuid_selected, valve_address)
                        time.sleep(1)
                        self.setup_addresses_new(valve_uuid_selected, valve_address)
                        time.sleep(1)

                        self.quiet_all_valves(device_list, valve_uuid_selected)

                        ser = serial.Serial(self._rs485_loc, 9600)
                        ser.timeout = 1
                        retry += 1
                        retry_of_retry += 1
                        read_packet = self.send_programming_info(line, ser, valve_address)
                        if expected_packet == read_packet[0:10]:
                            packet_sent = True
                            break
                    if packet_sent:
                        print(f"Packet eventually sent after {retry} retries")
                    else:
                        print(f"Packet never sent after {num_retries} retries exiting. Valve will be in a bad state." 
                              f"Please try to quiet RS485 and/or increase retries")
                        return
            except Exception:
                print(f"Exception: Unexpected packet read: {read_packet.hex('-')} vs {expected_packet.hex('-')} on line {line}")
            time.sleep(0.05)
            #time.sleep(1)
        ser.close()

    def send_programming_info(self, line, ser, valve_address):
        address = valve_address
        fw_packet = bytearray([0xFF, 0x0, 0xFF, 0xA5, 0x3F, address, 0xF, 0xF5, 0x2, 0x1, 0x2])
        print("programming 0x" + line["address"])
        data = line["address"] + "01" + line["data"]
        fw_packet[8] = 2 + 1 + 2 + line["byte_count"]
        fw_packet += bytearray.fromhex(data)
        self._calculate_and_append_checksum(fw_packet)
        ser.write(fw_packet)
        ser.flush()
        ser.timeout = 1
        read_packet: bytes = ser.read(size=12)
        return read_packet

    def goto_fw_update_mode_eggy(self, uuid: bytearray, address: int):
        print(f"Opening serial port: {self._rs485_loc}")
        ser = serial.Serial(self._rs485_loc, 9600)
        '''print(f'Forcing valve address: {hex(address)} uuid: {uuid.hex("-")} to fw update mode')
        fw_mode_packet = bytearray([0xFF, 0x0, 0xFF, 0xA5, 0x3F, address, 0x10, 0xF3, 0x7])
        fw_mode_packet += uuid
        fw_mode_packet += bytearray([0x1])
        self._calculate_and_append_checksum(fw_mode_packet)
        ser.write(fw_mode_packet)
        ser.flush()
        time.sleep(0.5)'''
        print(f'Forcing valve address: {hex(address)} uuid: {uuid.hex("-")} to reset')
        fw_mode_packet = bytearray([0xFF, 0x0, 0xFF, 0xA5, 0x3F, address, 0x10, 0xF1, 0x7])
        fw_mode_packet += uuid
        fw_mode_packet += bytearray([0x1])
        self._calculate_and_append_checksum(fw_mode_packet)
        ser.write(fw_mode_packet)
        ser.flush()
        print(f'Closing Port')
        ser.close()

    def select_valve_for_update(self, device_list: list) -> tuple:
        menu_list = {}
        menu_display_list = []
        index = 1
        for valve in device_list:
            menu_list[str(index)] = valve
            try:
                menu_display_list.append({"Index": index, "Address": valve['Address'], "UUID": valve["UUID"],
                                          "Git Hash": valve["Git Hash"], "Git Date": valve["Git Date"],
                                          "Branch": valve["Branch"], "Tag": valve["Tag"], "DID": valve["DID"],
                                          "RID": valve["RID"]})
            except Exception:
                menu_display_list.append({"Index": index, "Address": valve['Address'], "UUID": valve["UUID"],
                                          "Git Hash": "N/A", "Git Date": "",
                                          "Branch": "", "Tag": "", "DID": "",
                                          "RID": ""})
            index += 1

        menu_display_list.append({"Index": index, "Address": "", "UUID": "Valve Not found",
                                  "Git Hash": "N/A", "Git Date": "",
                                  "Branch": "", "Tag": "", "DID": "",
                                  "RID": ""})
        menu_list[str(index)] = {}
        print(tabulate(menu_display_list, headers='keys'))

        choice = input('Please select the index of the Valve for FW update: ')
        menu_choice = menu_list.get(choice, None)
        if menu_choice is None:
            print(f"{str(menu_choice)} was invalid; exiting")
            return None, "", 0
        if not menu_choice: # {}
            menu_choice["UUID"] = input('Please enter the valve UUID in format xx-yy-zz-uu-tt-ss: ')
            menu_choice["Type"] = input(f'Enter {EGGY_FW_TYPE} for new valve FW or {PENTAIR_FW_TYPE} for pentair FW: ')
            menu_choice["Address"] = input('Enter Address in Hex format (i.e. 0xA0): ')
        valve_uuid = menu_choice["UUID"]
        print(f'Valve {valve_uuid} was selected.')
        return bytearray.fromhex(valve_uuid.replace("-", "")), menu_choice["Type"], int(menu_choice["Address"], 16)

    def get_latest_firmware(self) -> str:
        response = requests.get("https://api.github.com/repos/jeffegg/EggysIVFW/releases/latest")
        if "name" in response.json().keys():
            print(f'Found latest FW version is: {response.json()["name"]}')
        else:
            print("No release found, please download manually")
            return ""

        download_url = ""
        for asset in response.json()["assets"]:
            if ("EggysIVFW__" in asset["browser_download_url"]) and (".hex" in asset["browser_download_url"]):
                download_url = asset["browser_download_url"]
                print(f'Found FW: {asset["browser_download_url"]}')
        if download_url == "":
            print("No FW found, please download manually")
            return ""

        fw_filename = download_url.split("/")[-1]
        tmp_path = os.path.join(tempfile.gettempdir(), '.{}'.format(hash(os.times())))
        os.makedirs(tmp_path)

        fw_full_path = os.path.join(tmp_path, fw_filename)

        r = requests.get(download_url, allow_redirects=True)
        with open(fw_full_path, 'wb') as file:
            file.write(r.content)
            file.close()

        return fw_full_path

    def list_firmware(self, get_fw=True) -> str:
        response = requests.get("https://api.github.com/repos/jeffegg/EggysIVFW/releases")
        index = 1
        menu_list = {}
        display_list = []
        for release_json in response.json():
            download_url = ""
            for asset in release_json["assets"]:
                if ("EggysIVFW__" in asset["browser_download_url"]) and (".hex" in asset["browser_download_url"]):
                    download_url = asset["browser_download_url"]
            display_list.append({"Index": index, "ID": hex(release_json["id"]), "Tag": release_json["tag_name"],
                                 "Name": release_json["name"], "Prerelease": release_json["prerelease"],
                                 "Publish Date": release_json["published_at"],
                                 "Has FW release": str(download_url != "")})
            menu_list[str(index)] = download_url
            #print(release_json["body"])
            index += 1
        print(tabulate(display_list, headers="keys"))
        if not get_fw:
            return ""

        if get_fw:
            choice = input('Please select the fw release: ')
            menu_choice = menu_list.get(choice, None)
            if menu_choice is None:
                #return self.get_latest_firmware()
                return ""
            download_url = menu_choice

            fw_filename = download_url.split("/")[-1]
            tmp_path = os.path.join(tempfile.gettempdir(), '.{}'.format(hash(os.times())))
            os.makedirs(tmp_path)

            fw_full_path = os.path.join(tmp_path, fw_filename)

            r = requests.get(download_url, allow_redirects=True)
            with open(fw_full_path, 'wb') as file:
                file.write(r.content)
                file.close()

            return fw_full_path

    def select_com_port(self) -> str:
        comport_list = serial.tools.list_ports.comports()
        index = 1
        comport_display_list = []
        menu_list = {}
        for comport in comport_list:
            entry = {"Index": index, "Device": comport.device, "Product": comport.product, "Serial #": comport.serial_number, "HW ID": comport.hwid}
            comport_display_list.append(entry)
            menu_list[str(index)] = entry
            index += 1

        print(tabulate(comport_display_list, headers='keys'))
        print(f"Default RS485 device is (hit enter to select): {self._rs485_loc}")
        choice = input('Please select the index of your RS485 device: ')
        menu_choice = menu_list.get(choice, None)
        if menu_choice is None:
            print(f"Default device of {self._rs485_loc} was selected.")
            return self._rs485_loc
        print(f"Device {self._rs485_loc} was selected.")
        return menu_choice["Device"]

    def find_all_valves(self, enumerate_timeout=50):
        print(f"Opening serial port: {self._rs485_loc}")
        ser = serial.Serial(self._rs485_loc, 9600)
        print("Forcing all address reset on all Pentair FW Valves")
        address_reset = bytearray([0xFF, 0x0, 0xFF, 0xA5, 0x3F, 0xC, 0x10, 0x51, 0x1, 0x1])
        self._calculate_and_append_checksum(address_reset)
        ser.write(address_reset)
        ser.flush()
        ser.close()
        print("Waiting 5 seconds for message to send and valves to quiet a bit")
        time.sleep(5)
        print("Sending ID valves packet to Valves with Eggy's Intellivalve Firmware")
        ser = serial.Serial(self._rs485_loc, 9600)
        id_valve_packet = bytearray([0xFF, 0x0, 0xFF, 0xA5, 0x3F, 0xF, 0xF, 0x12, 0x1, 0x1])
        self._calculate_and_append_checksum(id_valve_packet)
        ser.write(id_valve_packet)
        ser.flush()
        ser.close()
        time.sleep(0.5)
        ser = serial.Serial(self._rs485_loc, 9600)
        id_valve_packet = bytearray([0xFF, 0x0, 0xFF, 0xA5, 0x3F, 0xF, 0xF, 0x12, 0x1, 0x1])
        self._calculate_and_append_checksum(id_valve_packet)
        ser.write(id_valve_packet)
        ser.flush()
        ser.close()
        time.sleep(0.5)
        ser = serial.Serial(self._rs485_loc, 9600)
        id_valve_packet = bytearray([0xFF, 0x0, 0xFF, 0xA5, 0x3F, 0xF, 0xF, 0x12, 0x1, 0x1])
        self._calculate_and_append_checksum(id_valve_packet)
        ser.write(id_valve_packet)
        ser.flush()
        ser.close()
        print("Waiting 5 seconds for message to send and valves to quiet a bit")
        time.sleep(5)

        print("Listening to " + self._rs485_loc + " for " + str(enumerate_timeout) + " seconds.")
        ser = serial.Serial(self._rs485_loc, 9600)
        ser.timeout = enumerate_timeout
        all_valve_packets_by_uuid = []
        elapsed_time = 0
        last_time = time.time()

        while elapsed_time < enumerate_timeout:
            ser.flushInput()
            try:
                valve: bytearray = ser.read(size=10000)
                id_packets = []
                for i in range(len(valve) - 3):
                    if valve[i+0:i+3] == self._start_of_frame:
                        packet_len = int(valve[i+8:i+9].hex(), 16)
                        id_packets.append(valve[i : i+packet_len+9+2])
                '''print(valve)
                for entry in id_packets:
                    print(hex(len(entry)), entry)'''

                for entry in id_packets:
                    try:
                        new_valve_id = {}
                        new_valve_id = self.decode_valve_hail(entry)
                        shoud_add = True
                        for decoded_packet in all_valve_packets_by_uuid:
                            if new_valve_id['UUID'] == decoded_packet['UUID']:
                                shoud_add = False
                                break
                        if shoud_add:
                            all_valve_packets_by_uuid.append(new_valve_id)
                    except Exception:
                        try:
                            read_id = self.decode_unique_id(entry)
                            shoud_add = True
                            for decoded_packet in all_valve_packets_by_uuid:
                                if read_id.hex('-') == decoded_packet['UUID']:
                                    shoud_add = False
                                    break
                            if shoud_add:
                                all_valve_packets_by_uuid.append({"Type": PENTAIR_FW_TYPE, "Address": hex(entry[6]), "UUID": read_id.hex('-')})
                        except:
                            continue

            except BufferError:
                pass
            elapsed_time = time.time() - last_time
        print(f"Closing Serial Port: {self._rs485_loc}")
        ser.close()
        print(tabulate(all_valve_packets_by_uuid, headers='keys'))

        return all_valve_packets_by_uuid




