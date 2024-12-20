import csv
import json
import re
from pathlib import Path
from typing import TextIO, List

from constants import *
from string_chunks import *

class CSVProcessor:
    def __init__(self):
        self.csv_reader: csv.reader = None

    def open_reader(self, textio: TextIO):
        #Open a CSV reader, skip header
        self.csv_reader = csv.reader(textio)
        next(self.csv_reader)  # Skip the headers

    def process_defines(self):
        try:
            with open(INPUT_DEFINES, 'r') as input_file:
                self.open_reader(input_file)
                python_lines = ""
                for row in self.csv_reader:
                    if row[LINE_DISABLED]:
                        continue
                    python_lines += f"{row[DEF_NAME]} = {row[DEF_ID]}\n"
                with open(PYTHON_DATA, 'a') as python_file:
                    python_file.write(python_lines)
            print("Processed Defines")
        except (FileNotFoundError, ValueError) as e:
            print(f"Error processing defines: {e}")

    def process_items(self):
        try:
            with open(INPUT_ITEMS, 'r') as input_file:

                self.open_reader(input_file)

                python_lines = pyItemHeader + "\n"
                tracker_lines = "ITEM_MAPPING = { \n"
                client_lines = "const std::unordered_map<int, int> item_frequencies = {\n"
                basic_items = "const std::unordered_map<int, int> basic_items = {\n"
                json_list = []

                for row in self.csv_reader:
                    if row[LINE_DISABLED]:
                        continue
                    itm_name = row[ITM_NAME]
                    itm_id = row[ITM_ID]
                    itm_obj = row[ITM_OBJ]
                    itm_category = row[ITM_CATEGORY]
                    itm_ic = row[ITM_IC]
                    itm_freq = row[ITM_FREQ]
                    itm_trackertype = row[ITM_TRACKERTYPE]

                    # 1: python ***********
                    python_lines += (f"    \"{itm_name}\" "
                                 f": LL_Item(ll_base_id + {itm_id}"
                                 f", LL_ItemCat.{itm_category}"
                                 f", IC.{itm_ic}")
                    if itm_freq:
                        python_lines += f", {itm_freq}"
                    python_lines += "),\n"

                    # 2: tracker items ***********
                    # Convert CSV row to JSON object
                    item_json = {
                        "name": itm_name,
                        "type": itm_trackertype,
                        "max_quantity": int(itm_freq) if itm_freq else None,
                        "img": f"images/items/game items/itm_{itm_name.lower()}.png".replace(" ", "_"),
                        "codes": itm_name
                    }
                    # Remove "max_quantity" if None (for "toggle" type)
                    if item_json["max_quantity"] is None:
                        del item_json["max_quantity"]
                    json_list.append(item_json)# to JSON list

                    # 3: tracker mappings ***********
                    tracker_lines += f"    [{LOONYLAND_BASE_ID + globals()[itm_id]}] = {{{{\"{itm_name}\", \"{itm_trackertype}\"}}}},\n"

                    # 4: client hamsandwich ***********
                    if itm_freq:
                        client_lines += f"{{{itm_id}, {itm_freq}}},\n"

                    basic_items += f"{{ {itm_id} , {itm_obj}}},\n"


                python_lines += "}\n"
                with open(PYTHON_DATA, 'a') as python_file:
                    python_file.write(python_lines)


                tracker_lines += "}\n"
                with open(TRACKER_ITEMS, 'w') as tracker_file:
                    json.dump(json_list, tracker_file, indent=4)
                with open(TRACKER_SCRIPTS_DATA, 'a') as tracker_file:
                    tracker_file.write(tracker_lines)

                client_lines += "};\n\n"
                basic_items += "};\n\n"
                with open(CLIENT_DATA, 'a') as client_file:
                    client_file.write(client_lines)
                    client_file.write(basic_items)

            print("Processed Items")
        except (FileNotFoundError, ValueError) as e:
            print(f"Error processing items: {e}")



    def process_regions(self):
        try:
            with open(INPUT_REGIONS, 'r') as input_file:
                self.open_reader(input_file)

                python_region_lines = pyRegionHeader + "\n"
                tracker_region_lines = "loonyland_region_table = {\n"


                json_output = []
                overworld = {
                    "name": "Overworld",
                    "children": []
                }

                for row in self.csv_reader:  # type: ignore
                    if row[LINE_DISABLED]:
                        continue

                    reg_name = row[REG_NAME]
                    reg_real = row[REG_REAL]
                    reg_map = row[REG_MAP]
                    reg_map_id = row[REG_MAPID]
                    reg_x = row[REG_XCOORD]
                    reg_y = row[REG_YCOORD]

                    # 1: python
                    python_region_lines += f"    \"{reg_name}\": LLRegion({reg_real}"
                    if reg_map:
                        python_region_lines += f', \"{reg_map}\"'
                    python_region_lines += "),\n"

                    # 2: tracker region data
                    tracker_region_lines += f"    [\"{reg_name}\"] = {{real={reg_real}"
                    if reg_map:
                        tracker_region_lines += f', map=\"{reg_map}\"'
                    tracker_region_lines += f"}},\n"

                    # 3: tracker locations setup
                    region_entry = {
                        "name": reg_name,
                        #"access_rules": [ f"$can_reach|{reg_name}" ],
                        "sections": [],
                        "map_locations": []
                    }
                    if reg_x:  # Only add map location if map is real
                        map_location = {
                            "map": "Overworld",
                            "x": (int(reg_x) + 1) * TILE_XSIZE,
                            "y": (int(reg_y) + 1) * TILE_YSIZE
                        }
                        region_entry["map_locations"].append(map_location)

                    overworld["children"].append(region_entry)

                python_region_lines += "}\n"
                with open(PYTHON_DATA, 'a') as python_file:
                    python_file.write(python_region_lines)

                tracker_region_lines += "}\n"
                with open(TRACKER_SCRIPTS_DATA, 'a') as tracker_lua_file:
                    tracker_lua_file.write(tracker_region_lines)

                json_output.append(overworld)
                with open(TRACKER_LOCATIONS, 'w') as tracker_loc_file:
                    json.dump(json_output, tracker_loc_file, indent=2)

            print("Processed Regions")
        except (FileNotFoundError, ValueError) as e:
            print(f"Error processing regions: {e}")

    @staticmethod
    def parse_conditions(input_string, input_format):
        if not input_string:
            return ""

        def replace_item(match):
            """Helper to replace individual items with their desired format."""
            item = match.group(1)
            quantity = match.group(2)
            if quantity:
                if input_format == "python" :
                    return f'state.has("{item}", player, {quantity})'
                if input_format == "lua":
                    return f'state:has("{item}", {quantity})'
            if input_format == "python":
                return f'state.has("{item}", player)'
            if input_format == "lua":
                return f'state:has("{item}")'

        def replace_function(match):
            """Helper to replace functions like can_reach."""
            function_name = match.group(1)
            arguments = match.group(2)
            if function_name=="has":
                return f'{function_name}({arguments})'
            if function_name=="can_reach":
                if input_format == "python":
                    return f'state.can_reach_region(\"{arguments}\", player)'
                if input_format == "lua":
                    return f"{function_name}(\"{arguments}\")"
            if input_format == "python":
                return f'{function_name}(state, player)'
            if input_format == "lua":
                return f"{function_name}(state)"

        # Replace items like "Hose"10 or "Fertilizer"
        item_pattern = r'"(.*?)"(\d*)'
        input_string = re.sub(item_pattern, replace_item, input_string)

        function_pattern = r'(\w+)\((.*?)\)'
        input_string = re.sub(function_pattern, replace_function, input_string)


        return input_string

    def process_locations(self):
        try:
            with open(INPUT_LOCATIONS, 'r') as input_file, \
                    open(TRACKER_LOCATIONS, 'r+') as tracker_file_locs:

                data = json.load(tracker_file_locs)

                self.open_reader(input_file)
                python_loc_lines = pyLocationHeader + "\n"
                python_rules_lines = pyRulesHeader + "\n"
                tracker_loc_json = ""
                tracker_loc_lines = """local PICKUP <const> = 0
loonyland_location_table = {\n"""
                tracker_rules_lines = "access_rules = {\n"
                tracker_mapping_lines = "LOCATION_MAPPING = { \n"
                hamsandwich_lines = "static locationData basic_locations[] = {"

                for row in self.csv_reader:
                    if row[LINE_DISABLED]:
                        continue

                    # python location file
                    location_name = row[LOC_NAME]
                    location_name_no_colon = location_name.replace(":", "")
                    location_type_normal = row[LOC_TYPE]
                    location_type = row[LOC_TYPE].upper()  # Assuming Type is mapped to an enum
                    location_id = row[LOC_ID]
                    location_map = row[LOC_MAP]
                    location_map_id = row[LOC_MAPID]
                    location_xcoord = row[LOC_XCOORD]
                    location_ycoord = row[LOC_YCOORD]
                    location_spec1 = row[LOC_SPEC1ID]
                    location_spec2 = row[LOC_SPEC2ID]
                    location_region = row[LOC_REGION]
                    location_comment = row[LOC_COMMENT]
                    location_logic = row[LOC_LOGIC]
                    location_glitch_logic = row[LOC_GLITCH_LOGIC]
                    location_peek_logic = row[LOC_PEEK_LOGIC]
                    location_override = row[LOC_OVERRIDE]
                    location_chat_codes = row[LOC_CHATCODES]

                    python_loc_lines += f"    \"{location_name}\": LL_Location(" \
                                    f"{location_id}, LL_LocCat.{location_type}, \"{location_region}\"),\n"

                    tracker_loc_lines += f"    [\"{location_name_no_colon}\"]  = {{id={location_id}, type={location_type}, region=\"{location_region}\"}},\n"

                    if location_logic:  # Check if there are any specific rules for the location
                        # python logic file - Rules generation
                        rules_line = f"        \"{location_name}\": lambda state: "
                        rules_line += self.parse_conditions(location_logic, "python")
                        rules_line += ",\n"
                        python_rules_lines += rules_line

                        #tracker
                        rules_line = f"        [\"{location_name_no_colon}\"] = function(state) return "
                        rules_line +=  self.parse_conditions(location_logic, "lua")
                        rules_line += " end,\n"
                        tracker_rules_lines += rules_line

                    #find location_map in children
                    if location_map == "Halloween Hill":
                        data[0]['children'].append({
                            "name": location_name_no_colon,
                            "sections": [{"ref": f"Overworld/{location_region}/{location_name_no_colon}",
                                         "name": f"{location_name_no_colon}"}],
                            "map_locations": [{
                            "map": "Overworld",
                            "x": (int(row[LOC_XCOORD]) + 1) * TILE_XSIZE,
                            "y": (int(row[LOC_YCOORD]) + 1) * TILE_YSIZE
                        }]
                        })
                    for child in data[0]['children']:
                        if child['name'] == location_region:
                            child['sections'].append({
                                "name": location_name_no_colon,
                                "access_rules": [[f"[$access|{location_name_no_colon}]",
                                f"$sequence_break|{location_name_no_colon}"],
                                f"{{$peek|{location_name_no_colon}}}"
                                ]
                                })

                        elif child['name'] == location_override:
                            child['sections'].append({
                                "ref": f"Overworld/{location_region}/{location_name_no_colon}",
                                "name": f"{location_name_no_colon}"
                            })

                    #tracker mapping
                    tracker_mapping_lines +=f"    [{LOONYLAND_BASE_ID + int(location_id)}] = {{{{\"@Overworld/{row[LOC_REGION]}/{location_name_no_colon}\"}}}},\n"

                    #client data hamsandwich
                    hamsandwich_lines += f"{{\"{location_name}\",\"{location_type_normal}\",{location_id},\"{location_map}\",{location_map_id},"
                    hamsandwich_lines += f"{location_xcoord},{location_ycoord},{location_spec1},{location_spec2},\"{location_region}\",{{{location_chat_codes}}}}},\n"

                tracker_file_locs.truncate(0)
                tracker_file_locs.seek(0)
                json.dump(data, tracker_file_locs, indent=2)

                tracker_loc_lines += "}\n"
                tracker_rules_lines += "}\n"
                tracker_mapping_lines += "}\n"
                with open(TRACKER_SCRIPTS_DATA, 'a') as tracker_lua:
                    tracker_lua.write(tracker_loc_lines + tracker_rules_lines + tracker_mapping_lines)

                python_loc_lines += "}\n"
                python_rules_lines += "    }\n"
                python_rules_lines += pyRulesFooter + "\n"
                with open(PYTHON_DATA, 'a') as python_file:
                    python_file.write(python_loc_lines)
                    python_file.write(python_rules_lines)


                hamsandwich_lines += "};\n\n"
                with open(CLIENT_DATA, 'a') as client_data:
                    client_data.write(hamsandwich_lines)

            print("Processed Locations")
        except (FileNotFoundError, ValueError) as e:
            print(f"Error processing locations: {e}")

    def process_entrances(self):
        try:
            with open(INPUT_ENTRANCES, 'r') as input_file:

                self.open_reader(input_file)
                python_ent_lines = pyEntranceHeader + "\n"
                tracker_ent_lines = luaEntranceHeader + "\n"
                tracker_ent_lines += "loonyland_entrance_table = {\n"

                for row in self.csv_reader:
                    if row[LINE_DISABLED]:
                        continue

                    entrance_source = row[ENT_SOURCE]
                    if entrance_source == "":
                        continue

                    entrance_sourceX = row[ENT_SOURCEX]
                    entrance_sourceY = row[ENT_SOURCEY]
                    entrance_end = row[ENT_END]
                    entrance_load = row[ENT_LOAD]
                    entrance_logic = row[ENT_LOGIC]

                    # python entrance file
                    python_ent_lines += f"        LL_Entrance(\"{entrance_source}\", \"{entrance_end}\""
                    if entrance_load:
                        python_ent_lines += f", True"
                    else:
                        python_ent_lines += f", False"
                    if entrance_logic:
                        python_ent_lines += ", lambda state: "
                        python_ent_lines += self.parse_conditions(entrance_logic, "python")
                    python_ent_lines += "),\n"

                    # 2: Tracker entrances
                    tracker_ent_lines += f"        ent_valid{{source = \"{entrance_source}\", dest = \"{entrance_end}\""
                    if entrance_logic:
                        tracker_ent_lines += ", rule = function(state) return "
                        tracker_ent_lines += self.parse_conditions(entrance_logic, "lua")
                        tracker_ent_lines += " end"
                    tracker_ent_lines += "},\n"

                python_ent_lines += "]\n"
                python_ent_lines += pyEntranceFooter + "\n"
                with open(PYTHON_DATA, 'a') as python_file:
                    python_file.write(python_ent_lines)

                tracker_ent_lines += "}\n"
                with open(TRACKER_SCRIPTS_DATA, 'a') as tracker_file:
                    tracker_file.write(tracker_ent_lines)

            print("Processed Entrances")
        except (FileNotFoundError, ValueError) as e:
            print(f"Error processing locations: {e}")


def main():
    Path("./HamSandwich").mkdir(parents=True, exist_ok=True)
    Path("./Poptracker").mkdir(parents=True, exist_ok=True)
    Path("./Python").mkdir(parents=True, exist_ok=True)

    with open(PYTHON_DATA, 'w') as python_file:
        python_file.write("#" + genericHeader + pyMainHeader)

    with open(TRACKER_SCRIPTS_DATA, 'w') as tracker_file:
        tracker_file.write("--"+genericHeader)

    with open(CLIENT_DATA, 'w') as client_file:
        client_file.write("//"+genericHeader)

    processor = CSVProcessor()
    processor.process_defines()
    processor.process_items()
    processor.process_regions()
    processor.process_locations()
    processor.process_entrances()






if __name__=="__main__":
    main()
