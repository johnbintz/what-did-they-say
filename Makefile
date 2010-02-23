.PHONY : test

test :
	phpunit --coverage-html coverage test
