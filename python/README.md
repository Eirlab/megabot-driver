
python3 sin.py
Ctrl^C

cat sin.log  | grep '^4;3' > in.data

set datafile separator ";"

plot "in.data" using (column(8)):(column(4)), "in.data" using (column(8)):(column(5))

plot "in.data" using (column(8)):(column(6)), "in.data" using (column(8)):(column(7))
