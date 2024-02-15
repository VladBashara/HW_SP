Команда для запуска:

clean.bat && gcc parent.c -o parent.exe && gcc p_child.c -o p_child.exe && parent.exe <filename> <N> <delay>

clean.bat && gcc parent.c -o parent.exe && gcc p_child.c -o p_child.exe && parent.exe array 2 0

For powershell:
.\clean.bat; gcc parent.c -o parent.exe; gcc p_child.c -o p_child.exe; .\parent.exe array 2 0

python3 gen.py <filename> <left_edge> <right_edge> <NumCount>
python3 gen.py generated 0 20 5