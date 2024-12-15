import csv
import json
import re
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
                    open(TRACKER_ITEMS, 'w') as tracker_file:

                self.open_reader(input_file)
                python_file.write(pyItemHeader + "\n")

                json_list = []

                for row in self.csv_reader:
                    self.row_data = row
                    # python
                    item_line = (f"    \"{row[ITM_NAME]}\" "
                                 f": LL_Item(ll_base_id + {row[ITM_ID]}"
                                 f", LL_ItemCat.{row[ITM_CAT]}"
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

                python_file.write("}")
                json.dump(json_list, tracker_file, indent=4)
        except (FileNotFoundError, ValueError) as e:
            print(f"Error processing items: {e}")



    def process_regions(self):
        try:
            with open(INPUT_REGIONS, 'r') as input_file, \
                    open(PYTHON_REGIONS, 'w') as python_file:
                self.open_reader(input_file)
                python_file.write(pyRegionHeader + "\n")
                for row in self.csv_reader:  # type: ignore
                    self.row_data = row
                    print(row)
                    region_line = f"    \"{row[REG_NAME]}\": LLRegion({row[REG_REAL]}"
                    if row[REG_MAP]:
                        region_line += f', \"{row[REG_MAP]}\"'
                    region_line += "),\n"
                    python_file.write(region_line)


                python_file.write("}")

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


    def process_locations(self):
        try:
            with open(INPUT_LOCATIONS, 'r') as input_file, \
                    open(INPUT_LOCATIONS_TRACKER, 'r') as input_file_tracker, \
                    open(PYTHON_LOCATIONS, 'w') as python_file, \
                    open(PYTHON_RULES, 'w') as python_file_rules, \
                    open(TRACKER_LOCATIONS, 'w') as tracker_file:

                tracker_dict = {}
                data_lines = input_file_tracker.readlines()[2:]
                # Process each line
                for line in data_lines:
                    # Split the line into components (name, x, y)
                    name, x, y = line.strip().split(',')
                    # Add to the dictionary with x and y converted to integers
                    tracker_dict[name] = (int(x) + 1, int(y) + 1)

                self.open_reader(input_file)
                python_file.write(pyLocationHeader + "\n")
                python_file_rules.write(pyRulesHeader + "\n")

                json_output = []
                overworld = {
                    "name": "Overworld",
                    "children": []
                }

                for row in self.csv_reader:
                    self.row_data = row

                    # python location file
                    location_name = row[LOC_NAME]
                    location_id = row[LOC_ID]
                    location_type = row[LOC_TYPE].upper()  # Assuming Type is mapped to an enum
                    location_region = row[LOC_REGION]
                    location_logic = row[LOC_LOGIC]

                    location_line = f"    \"{location_name}\": LL_Location(" \
                                    f"{location_id}, LL_LocCat.{location_type}, \"{location_region}\"),\n"

                    python_file.write(location_line)

                    # python logic file - Rules generation
                    if location_logic:  # Check if there are any specific rules for the location
                        rules_line = f"        \"{location_name}\": lambda state: "
                        conditions = location_logic.split(" and ")
                        conditions_str = " and ".join([
                            self.parse_condition(cond.strip()) for cond in conditions])
                        conditions_str_tracker = ",".join([
                            self.parse_condition_tracker(cond.strip()) for cond in conditions])
                        rules_line += conditions_str
                        rules_line += ",\n"
                        python_file_rules.write(rules_line)

                    # Tracker locations - JSON creation
                    entry = {
                        "name": location_name,
                        "access_rules": [],
                        "sections": [
                            {"name": location_name}
                        ],
                        "map_locations": []
                    }

                    region_name = ""

                    if (row[LOC_TYPE]=="Pickup" and row[LOC_MAP]=="Halloween Hill") or (
                            row[LOC_TYPE]=="Quest" and not row[LOC_OVERRIDE]):
                        entry["map_locations"].append({
                            "map": "Overworld",
                            "x": (int(row[LOC_XCOORD]) + 1) * TILE_XSIZE,
                            "y": (int(row[LOC_YCOORD]) + 1) * TILE_YSIZE
                        })
                        region_name = row[LOC_NAME]
                    elif not row[LOC_OVERRIDE]:
                        key = row[LOC_MAP]
                        region_name = key
                        if key in tracker_dict:
                            x, y = tracker_dict[key]
                            entry["map_locations"].append({
                                "map": "Overworld",
                                "x": (int(x))*TILE_XSIZE,
                                "y": (int(y))*TILE_YSIZE
                            })
                    else:
                        key = row[LOC_OVERRIDE]
                        region_name = key
                        if key in tracker_dict:
                            x, y = tracker_dict[key]
                            entry["map_locations"].append({
                                "map": "Overworld",
                                "x": (int(x))*TILE_XSIZE,
                                "y": (int(y))*TILE_YSIZE
                            })


                    # Append entry to the corresponding region in the JSON
                    region_entry = next((child for child in overworld["children"] if
                                         child["name"]==region_name), None)
                    if not region_entry:
                        region_entry = {
                            "name": region_name,
                            "access_rules": [],
                            "sections": [],
                            "map_locations": []
                        }
                        overworld["children"].append(region_entry)
                        region_entry["map_locations"].extend(entry["map_locations"])

                    region_entry["sections"].append({"name": location_name,
                                                     #"access rules": condition_str_tracker
                                                     })

                json_output.append(overworld)

                # Write the JSON file
                with open(TRACKER_LOCATIONS, 'w') as tracker_json_file:
                    json.dump(json_output, tracker_json_file, indent=2)

                python_file.write("}")
                python_file_rules.write("    }\n")
                python_file_rules.write(pyRulesFooter)

        except (FileNotFoundError, ValueError) as e:
            print(f"Error processing locations: {e}")

    def process_entrances(self):
        try:
            with open(INPUT_ENTRANCES, 'r') as input_file, \
                    open(PYTHON_ENTRANCES, 'w') as python_file:

                self.open_reader(input_file)
                python_file.write(pyEntranceHeader + "\n")

                for row in self.csv_reader:
                    self.row_data = row

                    # python entrance file
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

                python_file.write("]\n")
                python_file.write(pyEntranceFooter)

        except (FileNotFoundError, ValueError) as e:
            print(f"Error processing locations: {e}")


def main():
    processor = CSVProcessor()
    processor.process_defines()
    processor.process_items()
    processor.process_regions()
    processor.process_locations()
    processor.process_entrances()


if __name__=="__main__":
    main()
