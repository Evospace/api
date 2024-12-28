import os
import re

def extract_class_details(file_content):
    print("Extracting class details from file content...")
    class_pattern = r'//class:\s*(\w+),\s*parent:\s*(\w+)'
    properties_pattern = r'\\.addProperty\\("(\w+)",\\s*&\\w+::(\w+)\\)\\s*//field:\\s*(\w+)'
    functions_pattern = r'\\.add(Function|StaticFunction)\\("(\w+)",\\s*&\\w+::(\w+)\\)\\s*//param\\s*([^,]+(?:,\\s*[^,]+)*),\\s*return\\s*(\w+)'
    accessor_pattern = r'(EVO_CODEGEN_ACCESSOR|EVO_CODEGEN_INSTANCE)\((\w+)\)'
    static_pattern = r'(EVO_OWNER)\((\w+)\)'

    matches = re.finditer(class_pattern, file_content)
    annotations = []

    for match in matches:
        class_name = match.group(1)
        parent_class = match.group(2)

        print(f"Found class: {class_name}, Parent class: {parent_class}")

        body = file_content  # Scan the entire content for properties and functions
        properties = re.findall(properties_pattern, body)
        print(f"Found properties: {properties}")
        functions = re.findall(functions_pattern, body)
        print(f"Found functions: {functions}")

        annotation = f"--- @class {class_name} : {parent_class}\n"
        for prop_name, prop_var, prop_type in properties:
            annotation += f"--- @field {prop_name} {prop_type}\n"

        for func_type, func_name, func_var, func_params, func_return_type in functions:
            func_annotation_type = "@static" if func_type == "StaticFunction" else ""
            param_list = func_params.split(', ') if func_params else []
            param_annotations = ', '.join(param_list)
            annotation += f"--- @function {func_annotation_type} {func_name}\n--- @param {param_annotations}\n--- @return {func_return_type}\n"

        annotation += f"{class_name} = {{}}\n"

        # Add EVO_CODEGEN_ACCESSOR annotation if found
        accessor_matches = re.finditer(accessor_pattern, body)
        for accessor_match in accessor_matches:
            accessor_type = accessor_match.group(2)
            annotation += f"\n--- Creates a new {accessor_type} instance\n--- @param parent Instance Instance of parent\n--- @param new_name string The name of the instance\n--- @return {accessor_type}\nfunction {accessor_type}.new(parent, new_name) end\n"

        # Add EVOSPACE_CODEGEN_DB annotation if found
        static_matches = re.finditer(static_pattern, body)
        for static_match in static_matches:
            static_type = static_match.group(2)
            annotation += f"\n--- Creates a new {static_type} static object\n--- @param new_name string The name of the instance\n--- @return {static_type}\nfunction {static_type}.new(new_name) end\n"
            annotation += f"\n--- Searching for {static_type} in db\n--- @param name string The name of the object\n--- @return {static_type}\nfunction {static_type}.find(name) end\n"

        annotations.append(annotation)

    return annotations

def parse_api_files(folder_path):
    print(f"Parsing API files in folder: {folder_path}")
    lua_annotations = []

    for root, _, files in os.walk(folder_path):
        print(f"Entering folder: {root}")
        for file in files:
            if file.endswith(".h"):
                file_path = os.path.join(root, file)
                print(f"Processing file: {file_path}")
                with open(file_path, 'r') as f:
                    content = f.read()
                    lua_annotations.extend(extract_class_details(content))

    print("Completed parsing API files.")
    return lua_annotations

def generate_lua_stub_file(output_file, annotations):
    print(f"Generating Lua stub file: {output_file}")
    with open(output_file, 'w') as f:
        # Read and write the content of the common_api file
        with open("./Source/Evospace/Shared/common_api.lua", 'r') as common_api:
            f.write(common_api.read())
        f.write("\n\n-- end of common --\n\n")
        # Write the annotations
        for annotation in annotations:
            f.write(annotation + "\n")
    print(f"Lua stub file written to: {output_file}")

if __name__ == "__main__":
    api_folder = "./Source/Evospace/Shared/"
    output_file = "./Source/Evospace/Shared/api.lua"

    print("Starting Lua stub generation...")
    annotations = parse_api_files(api_folder)
    print("Annotations generated:", annotations)
    generate_lua_stub_file(output_file, annotations)
    print(f"Lua stub file generated at: {output_file}")
