import csv
import json
import re
from pathlib import Path
from typing import TextIO, List

from constants import *
from string_chunks import *

class CSVProcessor:
    def __init__(self):
        self.value_lens: List[str] = []
        self.row_data: List[str] = []
        self.csv_reader: csv.reader = None

    def print_spaces(self, index: int) -> str:
        """Calculate spaces based on value_lens and row_data."""
        return ' ' * (int(self.value_lens[index]) - len(self.row_data[index]))

    def open_reader(self, textio: TextIO):
        """
        Open a CSV reader, skip header, and extract value_lens from the second row.
        """
        self.csv_reader = csv.reader(textio)
        next(self.csv_reader)  # Skip the headers
        self.value_lens = next(self.csv_reader)

    def process_defines(self):
        try:
            with open(INPUT_DEFINES, 'r') as input_file, \
                    open(PYTHON_DEFINES, 'w') as python_file:
                self.open_reader(input_file)
                for row in self.csv_reader:
                    self.row_data = row
                    python_file.write(f"{row[0]}{self.print_spaces(0)}= {row[1]}\n")
        except (FileNotFoundError, ValueError) as e:
            print(f"Error processing defines: {e}")

    def process_items(self):
        try:
            with open(INPUT_ITEMS, 'r') as input_file, \
                    open(PYTHON_ITEMS, 'w') as python_file, \
                    open(TRACKER_ITEM_MAPPING, 'w') as tracker_map_file, \
                    open(TRACKER_ITEMS, 'w') as tracker_file, \
                    open(CLIENT_DATA, 'w') as client_file:

                self.open_reader(input_file)
                python_file.write(pyItemHeader + "\n")
                tracker_map_file.write("ITEM_MAPPING = { \n")
                client_file.write("const std::unordered_map<int, int> item_frequencies = {\n")
                basic_items = "const std::unordered_map<int, int> basic_items = {\n"

                json_list = []

                for row in self.csv_reader:
                    self.row_data = row
                    # python
                    item_line = (f"    \"{row[ITM_NAME]}\" "
                                 f": LL_Item(ll_base_id + {row[ITM_ID]}"
                                 f", LL_ItemCat.{row[ITM_CATEGORY]}"
                                 f", IC.{row[ITM_IC]}")
                    if row[ITM_FREQ]:
                        item_line += f", {row[ITM_FREQ]}"
                    item_line += "),\n"
                    python_file.write(item_line)

                    # tracker
                    # TODO tracker
                    # Convert CSV row to JSON object
                    item_json = {
                        "name": row[ITM_NAME],
                        "type": "consumable" if row[ITM_FREQ] else "toggle",
                        "max_quantity": int(row[ITM_FREQ]) if row[ITM_FREQ] else None,
                        "img": f"images/items/game items/itm_{row[ITM_NAME].lower()}.png".replace(" ", "_"),
                        "codes": row[ITM_NAME]
                    }
                    # Remove "max_quantity" if None (for "toggle" type)
                    if item_json["max_quantity"] is None:
                        del item_json["max_quantity"]
                    json_list.append(item_json)# to JSON list

                    #tracker mappings
                    tracker_map_file.write(f"    [{LOONYLAND_BASE_ID + globals()[row[ITM_ID]]}] = {{{{\"{row[ITM_NAME]}\", \"{row[ITM_TRACKERTYPE]}\"}}}},\n")

                    #client hamsandwich
                    if row[ITM_FREQ]:
                        client_file.write(f"{{{row[ITM_ID]}, {row[ITM_FREQ]}}},\n")

                    basic_items += f"{{ {row[ITM_ID]} , {row[ITM_OBJ]}}},\n"


                python_file.write("}")
                tracker_map_file.write("}")
                client_file.write("};\n\n")
                basic_items += "};\n\n"
                client_file.write(basic_items)
                json.dump(json_list, tracker_file, indent=4)
        except (FileNotFoundError, ValueError) as e:
            print(f"Error processing items: {e}")



    def process_regions(self):
        try:
            with open(INPUT_REGIONS, 'r') as input_file, \
                    open(PYTHON_REGIONS, 'w') as python_file, \
                    open(TRACKER_REGIONS, 'w') as tracker_reg_file, \
                    open(TRACKER_LOCATIONS, 'w') as tracker_loc_file:
                self.open_reader(input_file)
                python_file.write(pyRegionHeader + "\n")
                tracker_reg_file.write("loonyland_region_table = {\n")


                json_output = []
                overworld = {
                    "name": "Overworld",
                    "children": []
                }

                for row in self.csv_reader:  # type: ignore
                    self.row_data = row
                    print(row)
                    #python
                    region_line = f"    \"{row[REG_NAME]}\": LLRegion({row[REG_REAL]}"
                    if row[REG_MAP]:
                        region_line += f', \"{row[REG_MAP]}\"'
                    region_line += "),\n"
                    python_file.write(region_line)

                    #tracker region data
                    region_line = f"    [\"{row[REG_NAME]}\"] = {{real={row[REG_REAL]}"
                    if row[REG_MAP]:
                        region_line += f', map=\"{row[REG_MAP]}\"'
                    region_line += f"}},\n"
                    tracker_reg_file.write(region_line)

                    #tracker locations setup
                    region_entry = {
                        "name": row[REG_NAME],
                        "access_rules": [ #f"$can_glitch|{row[REG_NAME]}",
                                            f"$can_reach|{row[REG_NAME]}" ],
                        "sections": [],
                        "map_locations": []
                    }
                    if row[REG_XCOORD]:  # Only add map location if map is real
                        map_location = {
                            "map": "Overworld",
                            "x": (int(row[REG_XCOORD]) + 1) * TILE_XSIZE,
                            "y": (int(row[REG_YCOORD]) + 1) * TILE_YSIZE
                        }
                        region_entry["map_locations"].append(map_location)

                    overworld["children"].append(region_entry)

                json_output.append(overworld)
                json.dump(json_output, tracker_loc_file, indent=2)

                python_file.write("}")
                tracker_reg_file.write("}")

        except (FileNotFoundError, ValueError) as e:
            print(f"Error processing regions: {e}")
    @staticmethod
    def parse_condition(condition):
        """Process a single condition, handling cases where there are function calls or quantity values."""

        # Check for a number after the condition (like "Mushroom"10)
        if condition[-1].isdigit():  # If it ends with a number
            match = re.search(r"\"([^\"]+)\"(\d+)$", condition)  # Match the condition and the number at the end
            if match:
                name = match.group(1)  # Condition name (like "Mushroom")
                quantity = int(match.group(2))  # Quantity value
                return f"state.has(\"{name}\", player, {quantity})"

        # Check if the condition corresponds to an item or a function call (e.g., have_bombs())
        if condition[0] == "\"":  # We check if it starts with "
            return f"state.has({condition}, player)"
            # Remove the quotes and format as function call
        return f"{condition}(state, player)"


    @staticmethod
    def parse_condition_lua(condition):
        """Process a single condition, handling cases where there are function calls or quantity values."""

        # Check for a number after the condition (like "Mushroom"10)
        if condition[-1].isdigit():  # If it ends with a number
            match = re.search(r"\"([^\"]+)\"(\d+)$", condition)  # Match the condition and the number at the end
            if match:
                name = match.group(1)  # Condition name (like "Mushroom")
                quantity = int(match.group(2))  # Quantity value
                return f"state:has(\"{name}\", {quantity})"

        # Check if the condition corresponds to an item or a function call (e.g., have_bombs())
        if condition[0] == "\"":  # We check if it starts with "
            return f"state:has({condition})"
            # Remove the quotes and format as function call
        return f"{condition}(state)"

    @staticmethod
    def tracker_parse_conditions(conditions):
        if not conditions:
            return ""
        # Split the condition by 'and'
        parts = conditions.split(" and ")

        # Process each part
        formatted_parts = []
        for part in parts:
            part = part.strip()  # Remove any extra spaces

            # If the part ends with a number, handle it as a value (e.g., Mushroom10 becomes Mushroom:10)
            if part[-1].isdigit():
                name = part.rstrip('0123456789')  # Remove digits
                number = part[len(name):]  # Get the number
                name = name.strip('"')
                formatted_parts.append((f"{name}:{number}"))  # Format as name:number
            elif part.startswith('"') and part.endswith('"'):
                # If the part starts and ends with quotes, keep it as is
                formatted_parts.append(part.strip('"'))  # Remove the quotes around the item
            else:
                # If it's a regular part, assume it's an identifier
                formatted_parts.append(f"${part}")  # Add dollar sign for variables

        # Join the formatted parts with commas
        return ", ".join(formatted_parts)


    def process_locations(self):
        try:
            with open(INPUT_LOCATIONS, 'r') as input_file, \
                    open(TRACKER_LOCATIONS, 'r+') as tracker_file_locs, \
                    open(TRACKER_LOCATIONS_DATA, 'w') as tracker_file_locs_data, \
                    open(TRACKER_RULES, 'w') as tracker_file_rules, \
                    open(TRACKER_LOCATION_MAPPING, 'w') as tracker_file_mapping, \
                    open(PYTHON_LOCATIONS, 'w') as python_file, \
                    open(PYTHON_RULES, 'w') as python_file_rules, \
                    open(CLIENT_DATA, 'a') as client_data:

                data = json.load(tracker_file_locs)

                self.open_reader(input_file)
                python_file.write(pyLocationHeader + "\n")
                python_file_rules.write(pyRulesHeader + "\n")
                tracker_file_locs_data.write("""local PICKUP <const> = 0
loonyland_location_table = {\n""")
                tracker_file_rules.write("access_rules = {\n")
                tracker_file_mapping.write("LOCATION_MAPPING = { \n")
                client_data.write("static locationData basic_locations[] = {")

                for row in self.csv_reader:
                    self.row_data = row

                    # python location file
                    location_name = row[LOC_NAME]
                    location_id = row[LOC_ID]
                    location_map = row[LOC_MAP]
                    location_map_id = row[LOC_MAPID]
                    location_xcoord = row[LOC_XCOORD]
                    location_ycoord = row[LOC_YCOORD]
                    location_spec1 = row[LOC_SPEC1ID]
                    location_spec2 = row[LOC_SPEC2ID]
                    location_type_normal = row[LOC_TYPE]
                    location_type = row[LOC_TYPE].upper()  # Assuming Type is mapped to an enum
                    location_region = row[LOC_REGION]
                    location_logic = row[LOC_LOGIC]
                    location_override = row[LOC_OVERRIDE]
                    location_chat_codes = row[LOC_CHATCODES]

                    location_line = f"    \"{location_name}\": LL_Location(" \
                                    f"{location_id}, LL_LocCat.{location_type}, \"{location_region}\"),\n"

                    python_file.write(location_line)

                    location_line = f"    [\"{location_name}\"]  = {{id={location_id}, type={location_type}, region=\"{location_region}\"}},\n"
                    tracker_file_locs_data.write(location_line)
                    conditions_str_tracker = ""

                    # python logic file - Rules generation
                    if location_logic:  # Check if there are any specific rules for the location
                        # python logic file - Rules generation
                        rules_line = f"        \"{location_name}\": lambda state: "
                        conditions = location_logic.split(" and ")
                        conditions_str = " and ".join([
                            self.parse_condition(cond.strip()) for cond in conditions])
                        conditions_str_tracker = ', '.join(f"${part}" if part.isidentifier() else part.strip('"').replace('"', '') if part.startswith('"') else f"{part.rstrip('0123456789').strip('"')}:{part[len(part.rstrip('0123456789')):]}" for part in location_logic.split(' and '))

                        rules_line += conditions_str
                        rules_line += ",\n"
                        python_file_rules.write(rules_line)

                        #tracker
                        rules_line = f"        [\"{location_name}\"] = function(state) return "
                        conditions = location_logic.split(" and ")
                        conditions_str = " and ".join([
                            self.parse_condition_lua(cond.strip()) for cond in conditions])

                        rules_line += conditions_str
                        rules_line += " end,\n"
                        tracker_file_rules.write(rules_line)




                    #find location_map in children
                    if location_map == "Halloween Hill":
                        data[0]['children'].append({
                            "name": location_name,
                            "sections": [{"ref": f"Overworld/{location_region}/{location_name}",
                                         "name": f"{location_name}"}],
                            "map_locations": [{
                            "map": "Overworld",
                            "x": (int(row[LOC_XCOORD]) + 1) * TILE_XSIZE,
                            "y": (int(row[LOC_YCOORD]) + 1) * TILE_YSIZE
                        }]
                        })
                    for child in data[0]['children']:
                        if child['name'] == location_region:
                            #child['access_rules'].append( f"@{location_region}")
                            child['sections'].append({
                                "name": location_name,
                                "access_rules": [f"{self.tracker_parse_conditions(location_logic)}"],
                                })
                            #add to sections[] with logic

                        elif child['name'] == location_override:
                            child['sections'].append({
                                "ref": f"Overworld/{location_region}/{location_name}",
                                "name": f"{location_name}"
                            })

                    #tracker mapping
                    tracker_file_mapping.write(f"    [{LOONYLAND_BASE_ID + int(location_id)}] = {{{{\"@Overworld/{row[LOC_REGION]}/{row[LOC_NAME]}\"}}}},\n")

                    #client data hamsandwich
                    client_data.write(f"{{\"{location_name}\",\"{location_type_normal}\",{location_id},\"{location_map}\",{location_map_id},")
                    client_data.write(f"{location_xcoord},{location_ycoord},{location_spec1},{location_spec2},\"{location_region}\",{{{location_chat_codes}}}}},\n")

                tracker_file_locs.truncate(0)
                tracker_file_locs.seek(0)
                json.dump(data, tracker_file_locs, indent=2)

                tracker_file_locs_data.write("}")
                tracker_file_rules.write("}")
                tracker_file_mapping.write("}")
                python_file.write("}")
                python_file_rules.write("    }\n")
                python_file_rules.write(pyRulesFooter)

                client_data.write("};\n\n")

        except (FileNotFoundError, ValueError) as e:
            print(f"Error processing locations: {e}")

    def process_entrances(self):
        try:
            with open(INPUT_ENTRANCES, 'r') as input_file, \
                    open(PYTHON_ENTRANCES, 'w') as python_file, \
                    open(TRACKER_ENTRANCES, 'w') as tracker_file:

                self.open_reader(input_file)
                python_file.write(pyEntranceHeader + "\n")
                tracker_file.write("loonyland_entrance_table = {\n")

                for row in self.csv_reader:
                    self.row_data = row

                    entrance_disable = row[ENT_DISABLE]
                    entrance_source = row[ENT_SOURCE]

                    if entrance_disable == "#" or entrance_source == "":
                        continue

                    entrance_source = row[ENT_SOURCE]
                    entrance_sourceX = row[ENT_SOURCEX]
                    entrance_sourceY = row[ENT_SOURCEY]
                    entrance_end = row[ENT_END]
                    entrance_load = row[ENT_LOAD]
                    entrance_logic = row[ENT_LOGIC]
                    #         EntranceData("Swamp Gas Cavern Front", "Swamp Gas Cavern Back", False,
                    #                      lambda state: state.has("Boots", player)),  # one way

                    # python entrance file
                    entrance_line = f"        LL_Entrance(\"{entrance_source}\", \"{entrance_end}\""
                    if entrance_load:
                        entrance_line += f", True, "
                    else:
                        entrance_line += f", False, "
                    entrance_line += "lambda state: "
                    if entrance_logic:
                        conditions = entrance_logic.split(" and ")
                        conditions_str = " and ".join([
                            self.parse_condition(cond.strip()) for cond in conditions])
                        entrance_line += conditions_str
                    else:
                        entrance_line += "True"
                    entrance_line += "),\n"
                    python_file.write(entrance_line)

                    entrance_line = f"        {{source = \"{entrance_source}\", dest = \"{entrance_end}\", "
                    #if entrance_load:
                    #    entrance_line += f", True, "
                    # else:
                    #    entrance_line += f", False, "
                    entrance_line += "rule = function(state) return "
                    if entrance_logic:
                        conditions = entrance_logic.split(" and ")
                        conditions_str = " and ".join([
                            self.parse_condition_lua(cond.strip()) for cond in conditions])
                        entrance_line += conditions_str
                    else:
                        entrance_line += "true"
                    entrance_line += " end},\n"
                    tracker_file.write(entrance_line)


                python_file.write("]\n")
                tracker_file.write("}")
                python_file.write(pyEntranceFooter)

        except (FileNotFoundError, ValueError) as e:
            print(f"Error processing locations: {e}")


def main():
    Path("./HamSandwich").mkdir(parents=True, exist_ok=True)
    Path("./Poptracker").mkdir(parents=True, exist_ok=True)
    Path("./Python").mkdir(parents=True, exist_ok=True)

    processor = CSVProcessor()
    processor.process_defines()
    processor.process_items()
    processor.process_regions()
    processor.process_locations()
    processor.process_entrances()


if __name__=="__main__":
    main()
