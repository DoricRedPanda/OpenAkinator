# OpenAkinator

Think about some character. This game will try to guess your character by
asking yes/no questions. OpeanAkinator is good at finding right questions and
learning from his mistakes.

# Building

To build open-akinator run:
```
make
```
or
```
make open-akinator
```

Actually, there is a debug version with verbose output. You can use it to learn
about akinator's calculations. To build run:
```
make debug
```

# Usage
If you want to play with existing database, then enter files where data is
stored. Example:
```
./open-akinator res/persons.txt res/questions.txt
```
or
```
./run.sh
```
If you want to create empty database, then just run program without arguments
and answer the questions. Example:
```
./open-akinator
```
To answer a question, enter "y", "n", or "?" symbol. After the game OpenAkinator
may ask you to teach him some new characters or questions.

# Math:

* [Bayes Rule](https://arbital.com/p/bayes_rule/)
* [Entropy](https://en.wikipedia.org/wiki/Entropy_(information_theory))

# Have fun!
