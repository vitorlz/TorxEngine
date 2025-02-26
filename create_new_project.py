import os
import shutil
import sys
from pathlib import Path

def update_file(file_path: Path, old_placeholder: str, new_name: str):
    if not file_path.exists():
        print(f"Warning: {file_path} does not exist.")
        return
    content = file_path.read_text()
    updated_content = content.replace(old_placeholder, new_name)
    file_path.write_text(updated_content)
    print(f"Updated {file_path} with project name '{new_name}'.")

def append_to_top_cmake(top_cmake: Path, subdirectory: str):
    if not top_cmake.exists():
        print(f"Top-level CMake file '{top_cmake}' not found.")
        return
    content = top_cmake.read_text()
    add_line = f"add_subdirectory({subdirectory})"
    if add_line in content:
        print(f"'{add_line}' already present in {top_cmake}.")
        return
    with top_cmake.open("a") as f:
        f.write("\n" + add_line + "\n")
    print(f"Appended '{add_line}' to {top_cmake}.")

def create_new_project(template_dir: Path, projects_root: Path, new_project_name: str):
    new_project_dir = projects_root / new_project_name

    if new_project_dir.exists():
        print(f"Error: Directory {new_project_dir} already exists!")
        sys.exit(1)

    shutil.copytree(template_dir, new_project_dir)
    print(f"Copied template from {template_dir} to {new_project_dir}")

    new_cmake = new_project_dir / "CMakeLists.txt"
    update_file(new_cmake, "PROJECT_NAME_TEMPLATE", new_project_name)

    main_cpp = new_project_dir / "src" / "main.cpp"
    update_file(main_cpp, "NEW_PROJECT_TITLE", new_project_name)

    return new_project_dir

if __name__ == "__main__":
    script_dir = Path(__file__).parent.resolve()

    template_project = script_dir / "Projects" / "template"  
    projects_root = script_dir / "Projects"

    top_level_cmake = script_dir / "CMakeLists.txt"

    if len(sys.argv) < 2:
        sys.exit(1)

    new_project_name = sys.argv[1].strip()
    if not new_project_name:
        print("Project name cannot be empty.")
        sys.exit(1)

    create_new_project(template_project, projects_root, new_project_name)

    subdir_path = f"Projects/{new_project_name}"
    append_to_top_cmake(top_level_cmake, subdir_path)

    print(f"New project '{new_project_name}' has been created successfully")
