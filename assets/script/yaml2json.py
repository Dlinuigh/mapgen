import yaml
import json
import sys

def yaml_to_json(yaml_file, json_file):
    try:
        with open(yaml_file, 'r', encoding='utf-8') as yf:
            data = yaml.safe_load(yf)
        
        with open(json_file, 'w', encoding='utf-8') as jf:
            json.dump(data, jf, ensure_ascii=False, indent=4)
        
        print(f"Successfully converted {yaml_file} to {json_file}")
    except Exception as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python yaml2json.py <input.yaml> <output.json>")
    else:
        yaml_file = sys.argv[1]
        json_file = sys.argv[2]
        yaml_to_json(yaml_file, json_file)
