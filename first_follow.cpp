/* 
 * File:  first_follow.cpp
 * Author: apoorv
 *
 * Created on April 3rd 2012
 */

/*
 * Take note of the consts used in grammars, these are hard coded
 * 1. "EPSILON" - stands for the empty string
 * 2. "$" - stands for the end marker
 * 
 *
 * The first function supports detection of left recursion
 * on detection it gives the faulty production and then exits
 */


#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <sstream>
#include <set>
#include <utility>
#include <map>
using namespace std;


typedef  vector<string> ProductionRight;
typedef string ProductionLeft;
typedef multimap<ProductionLeft , ProductionRight> ProductionList;



class Grammar
{
private:
	ProductionList production_list;
	set<string> terminals;
	set<string> non_terminals;


	bool has_epsilon_in_production(string current_str)
	{
		//get all productions
		multimap<ProductionLeft , ProductionRight>::iterator curr_start = production_list.equal_range(current_str).first;
		multimap<ProductionLeft , ProductionRight>::iterator curr_end = production_list.equal_range(current_str).second;

		bool has_epsilon = false;
		for(multimap<ProductionLeft , ProductionRight>:: iterator current_prod = curr_start ; current_prod != curr_end ; current_prod++ )
		{
			if((*current_prod).second[0] == "EPSILON") has_epsilon = true;
		}

		return has_epsilon;
	}


public:
	
	void add_production( ProductionLeft pl , ProductionRight pr )
	{
		production_list.insert( pair<ProductionLeft , ProductionRight>(pl , pr) );
	}
	void add_terminal(string term)
	{
		terminals.insert(term);
	}
	void add_non_terminal(string non_term)
	{
		non_terminals.insert(non_term);
	}
	set<string> first(string element)
	{
		//first of epsilon is empty set
		if( element == "EPSILON" )
		{
			set<string> _first;
			_first.clear();
			//return empty set
			return _first;
		}
		//if it is one of the terminals
		else if( terminals.count(element) == 1 )
		{
			set<string> _first;
			_first.clear();
			_first.insert(element);
			//return singleton set with the element itself.
			return _first;
		}
		else // a non terminal
		{
			//get all productions with left as element
			multimap<ProductionLeft , ProductionRight>::iterator start = production_list.equal_range(element).first;
			multimap<ProductionLeft , ProductionRight>::iterator end = production_list.equal_range(element).second;


			//for each production
			//collect first elements in _first
			set<string> _first;
			_first.clear();
			for(multimap<ProductionLeft , ProductionRight>::iterator prod = start ; prod != end ; prod++)
			{			
				bool is_epsilon = true; // assume that all have epsilon in their start
				int i =0 ;

				//check for left recursion
				if ( (*prod).second[0] == element)
				{
					cerr << "ERROR: left recursion found\n";
					cerr << "production: " ;
					cerr << element << " -> ";
					for ( ProductionRight:: iterator i = (*prod).second.begin() ; i != (*prod).second.end() ; i++)
						cerr << *i << " ";
					cerr << endl;
					exit(1);
				}


				while(true)
				{
					string current_str = (*prod).second[i];
					set<string> temp_first = first(current_str);
					_first.insert(temp_first.begin() , temp_first.end());

					//check if the element is EPSILON
					if( current_str == "EPSILON") break;
					//or terminal
					else if(terminals.count(current_str) == 1)
					{
						//cannot be epsilon
						is_epsilon = false;
						//no point in continuing
						break;
					}
					//check if the current element has epsilon in it's production
					else
					{
						//get all productions
						//TODO - use the is_epsilon(str) function
						multimap<ProductionLeft , ProductionRight>::iterator curr_start = production_list.equal_range(current_str).first;
						multimap<ProductionLeft , ProductionRight>::iterator curr_end = production_list.equal_range(current_str).second;

						bool has_epsilon = false;
						for(multimap<ProductionLeft , ProductionRight>:: iterator current_prod = curr_start ; current_prod != curr_end ; current_prod++ )
						{
							if((*current_prod).second[0] == "EPSILON") has_epsilon = true;
						}

						if(!has_epsilon)
						{
							//cannot be epsilon
							is_epsilon = false;
							//no point in continuing
							break;
						}
						else continue;
					}
					//update index of current element
					i++;

					//check if it is in bounds
					if(i >= (*prod).second.size() ) break;
				}

				// if all elements had an epsilon option
				if(is_epsilon)
				{
					_first.insert("EPSILON");
				}
				

			}


			return _first;
		}
	}

	set<string> follow(string element)
	{
		if( terminals.count(element) == 1) cerr << " follow not defined for terminals\n";

		//create set that will hold all follows
		set<string> _follow;
		_follow.clear();

		//for each production in set
		for( ProductionList::iterator current_production = production_list.begin() ; current_production != production_list.end() ; current_production++)
		{
			//for each term in production
			for( ProductionRight:: iterator current_term = (*current_production).second.begin() ; current_term != (*current_production).second.end() ; current_term++)
			{
				//if current term matches the term of whose follow we are looking for
				if ( (*current_term) == element )
				{
					//look into all subsequent terms - until find find non EPSILON production - or the end
					ProductionRight:: iterator nxt_term = current_term+1;
					//assume that $ can be reached
					bool can_reach_$ = true;
					//loop until you reach end
					while( nxt_term !=  (*current_production).second.end() )
					{

						set<string> first_nxt_term = first( (*nxt_term) );
						_follow.insert( first_nxt_term.begin() , first_nxt_term.end() );

						if(  has_epsilon_in_production( (*nxt_term ) ) )
						{
							//update if epsilon
							nxt_term++;
						}
						else
						{
							//break on finding first non epsilon
							//cannot reach $
							can_reach_$= false;
							break;
						}
					}

					//check if $ can be reached , ie all are epsilon possible in  a row
					if(can_reach_$)
					{
						_follow.insert("$");
					}


					
					

				}
			}
		}

		return _follow;
	}

	void print_follow(string element)
	{
		set<string> s = follow(element);

		cout << "FOLLOW - " << element << " : " <<endl;
		for( set<string>:: iterator str= s.begin() ; str != s.end() ; str++)
		{
			cout << (*str) << " , ";
		}
		cout <<endl << endl;
	}

	void print_first(string element)
	{
		set<string> s = first(element);

		cout << "FIRST - " << element << " : " <<endl;
		for( set<string>:: iterator str= s.begin() ; str != s.end() ; str++)
		{
			cout << (*str) << " , ";
		}
		cout << endl <<endl;
	}
};

int main()
{



	Grammar grm;

	/*
	 * creating grammar:
	 * A -> aBC
	 * B -> c | EPSILON
	 * C -> e | EPSILON
	 */
	grm.add_non_terminal("A");
	grm.add_non_terminal("B");
	grm.add_non_terminal("C");
	grm.add_terminal("a");
	grm.add_terminal("c");
	grm.add_terminal("e");

	// A -> aBC
	ProductionLeft pl = "A";
	ProductionRight pr;
	pr.push_back("a");
	pr.push_back("B");
	pr.push_back("C");

	grm.add_production(pl , pr);


	// B -> c
	pl = "B";
	pr.clear();
	pr.push_back("c");
	grm.add_production(pl , pr);


	//  B -> EPSILON
	pl = "B";
	pr.clear();
	pr.push_back("EPSILON");
	grm.add_production(pl , pr);

	 // C -> e 
	pl = "C";
	pr.clear();
	pr.push_back("e");
	grm.add_production(pl , pr);

	 // C -> EPSILON
	pl = "C";
	pr.clear();
	pr.push_back("EPSILON");
	grm.add_production(pl , pr);


	grm.print_first("A");
	grm.print_follow("B");

}

