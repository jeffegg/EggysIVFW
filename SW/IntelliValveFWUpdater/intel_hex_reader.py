from math import floor

class intel_hex_reader:
    def __init__(self, filename):
        self._filename = filename
        self._parse_file_list = []
        self._parse_64_byte_list = []

    def open(self):
        with open(self._filename, 'r') as i_hex_file:
            unparsed_lines = i_hex_file.readlines()

        for line in unparsed_lines:
            if len(line) == 0:
                continue
            new_element = self.parse_line(line)
            if len(new_element) != 0:
                self._parse_file_list.append(new_element)

    def parse_line(self, line: str) -> dict:
        if ':00000001FF' == line:
            return {}
        if '00000001FF' in line:
            return {}
        if ':' != line[0]:
            raise KeyError("Line didn't start with : as expected")
        byte_count = int(line[1:3], 16)
        address = line[3:7]
        record_type = int(line[7:9], 16)

        print(byte_count, address, record_type)
        if record_type != 0:
            raise Exception("This is not a data record as expected")
        end_of_data = (byte_count * 2) + 9
        data = line[9:end_of_data]
        checksum = line[end_of_data: end_of_data+2]

        return {"byte_count": byte_count, "address": address, "data": data, "checksum": checksum}

    def convert_to_64_byte_lines(self):
        new_line = {"byte_count": 0x40, "address": None, "data": "", "checksum": None}
        for line in self._parse_file_list:
            print(line)
            if new_line["address"] is None:
                if (int(line["address"], 16) % 0x40) == 0:
                    new_line["address"] = line["address"]
                else:
                    alinged_address = int(line["address"], 16) - (int(line["address"], 16) % 0x40)
                    new_line["address"] = '{0:0{1}X}'.format(alinged_address, 4)
                    needed_0_bytes = int(line["address"], 16) % 0x40
                    new_line["data"] = "00" * needed_0_bytes
            else:
                if ((int(line["address"], 16) - int(new_line["address"], 16)) / 0x40) > 1:
                    needed_0_bytes = (0x40 * 0x2) - len(new_line["data"])
                    new_line["data"] += "00" * int(floor(needed_0_bytes / 2))

                    self._parse_64_byte_list.append(new_line.copy())
                    if (int(line["address"], 16) % 0x40) == 0:
                        new_line["address"] = line["address"]
                    else:
                        alinged_address = int(line["address"], 16) - (int(line["address"], 16) % 0x40)
                        new_line["address"] = '{0:0{1}X}'.format(alinged_address, 4)
                        needed_0_bytes = int(line["address"], 16) % 0x40

                        new_line["data"] = "00" * needed_0_bytes
            new_line["data"] += line["data"]

            if len(new_line["data"]) == (0x40 * 2): # x2 due to byte being 2 nibbles
                self._parse_64_byte_list.append(new_line.copy())

                new_line = {"byte_count": 0x40, "address": None, "data": "", "checksum": None}

        if new_line["address"] is not None:
            needed_0_bytes = (0x40 * 0x2) - len(new_line["data"])
            new_line["data"] += "00" * int(floor(needed_0_bytes / 2))
            self._parse_64_byte_list.append(new_line.copy())

