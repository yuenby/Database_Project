make: clean
	gcc src/*.c -I include -o bin/main

test: reset make
	bin/main -n -f "test.db"

	bin/main -a "Mehmet Birgül,79. Mah.,1" -f "test.db"
	bin/main -a "Yusuf Ahmet,Gaziosmanpaşa Cad.,30" -f "test.db"
	bin/main -a "Elif,Mevlana Mah.,21" -f "test.db"
	bin/main -a "Fahri Kabataş,Mars Gez.,3451627" -f "test.db"

	bin/main -l -f "test.db"

clean:
	rm -f bin/*

reset:
	rm -f *.db
