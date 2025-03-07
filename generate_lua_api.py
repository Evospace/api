import os
import re

def extract_class_details(file_content):
    print("Extracting class details from file content...")
    class_pattern = r'//@class\s+(\w+)(?:\s*:\s*(\w+))?'
    properties_pattern = r'\.addProperty\(\s*"([^"]+)"\s*,\s*(.*?)\)\s*//@field\s*(.*)'
    direct_section_pattern = r'//direct:\s*((?:\s*//.*\n)+)'
    accessor_pattern = r'(EVO_CODEGEN_ACCESSOR)\((\w+)\)'
    instance_pattern = r'(EVO_CODEGEN_INSTANCE)\((\w+)\)'
    static_pattern = r'(EVO_CODEGEN_DB)\((\w+),\s*(\w+)\)'
    just_ue_object_pattern = r'(EVO_JUST_UE)\((\w+),\s*(\w+)\)'

    matches = re.finditer(class_pattern, file_content)
    annotations = {}

    for match in matches:
        class_name = match.group(1)
        parent_class = match.group(2) if match.group(2) else "Object"

        print(f"Found class: {class_name}, Parent class: {parent_class}")

        body = file_content
        properties = re.findall(properties_pattern, body)
        direct_sections = re.finditer(direct_section_pattern, body)

        annotation = f"--- @class {class_name} : {parent_class}\n"

        for prop_name, code, prop_comment in properties:
            prop_parts = prop_comment.split(" ", 1)
            prop_type = prop_parts[0].strip() if prop_parts else "unknown"
            comment_str = f" {prop_parts[1].strip()}" if len(prop_parts) > 1 else ""
            annotation += f"--- @field {prop_name} {prop_type}{comment_str}\n"
            print(f"field {prop_name} {prop_type} {comment_str}")

        annotation += f"{class_name} = {{}}\n"

        for section in direct_sections:
            comments = section.group(1)
            comments = re.sub(r'^\s*//', '', comments, flags=re.MULTILINE)  # Remove leading '//' and trim spaces
            annotation += f"\n{comments.strip()}\n"

        accessor_matches = re.finditer(accessor_pattern, body)
        for accessor_match in accessor_matches:
            accessor_type = accessor_match.group(2)
            annotation += f"\n--- Creates a new {accessor_type} instance\n--- @param parent Object Object of parent\n--- @param new_name string The name of the instance\n--- @return {accessor_type}\nfunction {accessor_type}.new(parent, new_name) end\n"
            annotation += f"\n--- Return {accessor_type} class object\n--- @return Class\nfunction {accessor_type}.get_class() end\n"
            annotation += f"\n--- Trying to cast Object into {accessor_type}\n--- @param object Object to cast\n--- @return {accessor_type}\nfunction {accessor_type}.cast(object) end\n"

        instance_matches = re.finditer(instance_pattern, body)
        for instance_match in instance_matches:
            instance_type = instance_match.group(2)
            annotation += f"\n--- Creates a new {instance_type} instance\n--- @param parent Object Object of parent\n--- @param new_name string The name of the instance\n--- @return {instance_type}\nfunction {instance_type}.new(parent, new_name) end\n"
            annotation += f"\n--- Creates a new {instance_type} instance\n--- @return {instance_type}\nfunction {instance_type}.new_simple() end\n"
            annotation += f"\n--- Return {instance_type} class object\n--- @return Class\nfunction {instance_type}.get_class() end\n"
            annotation += f"\n--- Trying to cast Object into {instance_type}\n--- @param object Object to cast\n--- @return {instance_type}\nfunction {instance_type}.cast(object) end\n"

        static_matches = re.finditer(static_pattern, body)
        for static_match in static_matches:
            static_type = static_match.group(2)
            annotation += f"\n--- Creates a new {static_type} static object\n--- @param new_name string The name of the object\n--- @return {static_type}\nfunction {static_type}.new(new_name) end\n"
            annotation += f"\n--- Searching for {static_type} in db\n--- @param name string The name of the object\n--- @return {static_type}\nfunction {static_type}.find(name) end\n"
            annotation += f"\n--- Return {static_type} class object\n--- @return Class\nfunction {static_type}.get_class() end\n"
            annotation += f"\n--- Trying to cast Object into {static_type}\n--- @param object Object to cast\n--- @return {static_type}\nfunction {static_type}.cast(object) end\n"

        annotations[class_name] = annotation

    return annotations

def parse_api_files(folder_path):
    print(f"Parsing API files in folder: {folder_path}")
    lua_annotations = {}
    for root, _, files in os.walk(folder_path):
        print(f"Entering folder: {root}")
        for file in files:
            if file.endswith(".h"):
                file_path = os.path.join(root, file)
                print(f"Processing file: {file_path}")
                with open(file_path, 'r', encoding='utf-8') as f:
                    content = f.read()
                    lua_annotations.update(extract_class_details(content))
    print("Completed parsing API files.")
    return lua_annotations

def generate_lua_stub_file(output_file, annotations):
    print(f"Generating Lua stub file: {output_file}")
    with open(output_file, 'w', encoding='utf-8') as f:
        with open("./Source/Evospace/Shared/common_api.lua", 'r', encoding='utf-8') as common_api:
            f.write(common_api.read())
        f.write("\n\n-- end of common --\n\n")
        for key, annotation in annotations.items():
            f.write(annotation + "\n")
    print(f"Lua stub file written to: {output_file}")

if __name__ == "__main__":
    api_folder = "./Source/Evospace/Shared/"
    output_file = "./Source/Evospace/Shared/api.lua"
    print("Starting Lua stub generation...")
    annotations = parse_api_files(api_folder)
    annotations = dict(sorted(annotations.items()))
    generate_lua_stub_file(output_file, annotations)
    print(f"Lua stub file generated at: {output_file}")
