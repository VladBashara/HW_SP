Команда для запуска:

sh clean.sh && gcc parent.c -o parent && gcc child.c -o child && ./parent <filename> <N> <delay>

sh clean.sh && gcc parent.c -o parent && gcc child.c -o child && ./parent array 2 0

python3 gen.py <filename> <left_edge> <right_edge> <NumCount>
python3 gen.py generated 0 20 5