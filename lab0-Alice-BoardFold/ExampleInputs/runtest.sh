g++ Alice-Main.cpp -o tAlice -I.
sh Alice-Test.sh > mysol.txt
diff mysol.txt jsol.txt | wc
