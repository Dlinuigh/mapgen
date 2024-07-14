find src include -name "*.cpp" -o -name "*.h" -o -name "*.hpp" -o -name "*.c" | while read file; do
  clang-format -i "$file" 
done