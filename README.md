# 📁 Folder Size

Program to calculate the size of a folder and all its subfolders, recursively. After execution, it generates a SQLite database storing the full folder structure.


## 🚀 How to Run

```bash
make
.\build\main.exe <path_to_folder>
```
## 🗄️ Database Structure

| Column             | Description                         |
|--------------------|-------------------------------------|
| `id`               | Unique identifier for each entry    |
| `path`             | Full path to the folder             |
| `size`             | Size in bytes, if can't access value is 0     |
| `parent_folder_id` | ID of the parent folder             |
