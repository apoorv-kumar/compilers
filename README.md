#LL1 and SLR parser 

See: http://eat-sleep-code.blogspot.in/p/my-coding-projects.html

MIT License, Apoorv Kumar, 2012


	We created a LL1 parser and SLR parser both able to take a grammar as input , parse it and then generate tables associated with it. Also we created a code checker/generator that took care of missing declarations and warned user about invalid data usage. and The first step was creating a tokenizer.

	
##Steps
	Supplying the code
	And the list of keywords , delimiters and
	operators.
	Our tokenizer prints the list of tokens and generates the tokenized code


	Now we created the parser that parsed the grammar depending upon its type.
	The grammars (set of productions) look something like

	S_dash # E
	E # T D
	D # + T D : epsilon
	T # F G
	G # * F G : epsilon
	F # ( E ) : id

	Then , the tool takes a code as input. After preprocessing is done, it parses the code and checks whether it is a valid code and outputs the LL1 table.

	Finally we created a java based code generator that took complex code and broke it down to basic (one left operand , 2 right operand) form that can be easily converted to assembly   .

	We give this machine dependent code to the code generator and it parses , fixes , issues errors and generates the machine independent
	code along with the token table
