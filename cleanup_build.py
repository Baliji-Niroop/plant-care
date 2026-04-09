import os
import shutil
from pathlib import Path

build_dir = Path(r'C:\Users\niroo\Downloads\PLANT-CARE\simulation\wokwi\build')

print("=== CLEANUP EXECUTION ===\n")
print(f"Target directory: {build_dir}\n")

# Show initial state
print("=== Initial contents ===")
items = sorted([f.name for f in build_dir.iterdir()])
for item in items:
    print(f"  {item}")

print("\n=== Deleting all items except .gitkeep ===")
deleted_items = []

for item in build_dir.iterdir():
    if item.name != '.gitkeep':
        try:
            if item.is_dir():
                shutil.rmtree(item)
            else:
                os.remove(item)
            deleted_items.append(item.name)
            print(f"  ✓ Deleted: {item.name}")
        except Exception as e:
            print(f"  ✗ Error deleting {item.name}: {e}")

print(f"\n=== Summary: Deleted {len(deleted_items)} items ===")

print("\n=== Final directory listing ===")
final_items = sorted([f.name for f in build_dir.iterdir()])
for item in final_items:
    print(f"  {item}")

print("\n=== Result: SUCCESS ✓ ===")
