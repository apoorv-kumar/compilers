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
 * 3. "TERMINAL" - is the productionLeft when first() of some terminal is requested
 *      This compulsion comes because first is recursive.
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
#include <iomanip>
using namespace std;


typedef  vector<string> ProductionRight;
typedef string ProductionLeft;
typedef pair<ProductionLeft , ProductionRight> Production;
typedef multimap<ProductionLeft , ProductionRight> ProductionList;

//one cell of table - set of productions 
typedef set<Production> TableEntry;
typedef map<string , TableEntry> Row;
typedef map<string , Row> Table;



class Grammar
{
private:
	ProductionList production_list;
	set<string> terminals;
	set<string> non_terminals;
	Table table;

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

        bool can_be_epsilon( string element)
        {
            if(terminals.count(element) == 1) return false;
            else
            {
                set< pair<string , Production> > first_of_element = first(element);

                for ( set< pair<string , Production> >:: iterator i = first_of_element.begin() ; i != first_of_element.end() ; i++ )
                {

                    if ( (*i).first == "EPSILON") return true;
                }

                //if none returned true
                return false;
            }
        }

	void print_production(Production p)
	{
		cout << p.first << " -> " ;
		for( Production::second_type::iterator i = p.second.begin() ;  i != p.second.end() ; i++)
		{
			cout << *i;
		}
		cout << endl;
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
	set< pair< string , Production> > first(string element)
	{
		//first of epsilon is empty set
		if( element == "EPSILON" )
		{
			set< pair<string , Production> > _first;
			_first.clear();
			//return empty set
			return _first;
		}
		//if it is one of the terminals
		else if( terminals.count(element) == 1 )
		{
			set< pair<string , Production> > _first;
                        Production p;
			_first.clear();
                        //some error here - see what
                        pair<string , Production> pr;
			pr.first = element;
			//cout << "terminal\n";
			_first.insert(  pr );
			//return singleton set with the element itself.
                        //since we have to include a production , we include an empty production
			return _first;
		}
		else // a non terminal
		{
			//get all productions with left as element
			multimap<ProductionLeft , ProductionRight>::iterator start = production_list.equal_range(element).first;
			multimap<ProductionLeft , ProductionRight>::iterator end = production_list.equal_range(element).second;


			//for each production
			//collect first elements in _first
			set< pair<string , Production> > _first;
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
					//cout << "Checking for: " << element <<endl;
					string current_str = (*prod).second[i];
					set< pair<string , Production> > temp_first = first(current_str);

                                        //for all element,production pairs returned by first,
                                        //just switch the associated production and insert
                                        for( set< pair<string , Production> > ::iterator str_prod = temp_first.begin(); str_prod != temp_first.end() ; str_prod++)
                                        {
						//cout << "inserting : " << (*str_prod).first << endl;
                                            _first.insert( pair<string , Production>( (*str_prod).first , *prod ) );
                                        }

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

					}
					//update index of current element
					i++;

					//check if it is in bounds
					if(i >= (*prod).second.size() ) break;
				}

				// if all elements had an epsilon option
				if(is_epsilon)
				{
					_first.insert( pair<string , Production>("EPSILON" , *prod ) );
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
					//look into all subsequent terms - until find find non-EPSILON NonTerminal - or the end
					ProductionRight:: iterator nxt_term = current_term+1;
					//assume that $ can be reached
					bool can_reach_$ = true;
					//loop until you reach end
					while( nxt_term !=  (*current_production).second.end() )
					{

						set< pair<string , Production> > first_nxt_term = first( (*nxt_term) );
                                                for( set< pair<string , Production> >:: iterator i = first_nxt_term.begin() ; i != first_nxt_term.end() ; i++)
                                                {
                                                    _follow.insert((*i).first);
                                                }

						if(  can_be_epsilon( (*nxt_term ) ) )
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
		set< pair<string , Production> > s = first(element);

		cout << "FIRST - " << element << " : " <<endl;
		for( set< pair<string , Production> >:: iterator str= s.begin() ; str != s.end() ; str++)
		{
			cout << setw(10) << (*str).first << " - FROM - " ;
			print_production((*str).second);
		}
		cout << endl <<endl;
	}



	void update_table()
	{
		table.clear();

		//create a row for each NT
		for( set<string>::iterator iter_non_term = non_terminals.begin() ; iter_non_term != non_terminals.begin() ; iter_non_term++)
		{
			//create empty row
			Row r;
			table.insert( pair<string, Row> (*iter_non_term , r ) );

		}


		for( ProductionList:: iterator iter_prod = production_list.begin() ; iter_prod != production_list.end() ; iter_prod++)
		{
			string non_term = (*iter_prod).first;

			//loop 1
			// for each term a in first(NT)
			set< pair<string , Production> > first_nt = first(non_term);

			for( set< pair<string , Production> >:: iterator iter_str_prod = first_nt.begin() ; iter_str_prod != first_nt.end() ; iter_str_prod++)
			{
				Production p = (*iter_str_prod).second;
				string a = (*iter_str_prod).first;
				//don't insert for EPSILON
				if (a != "EPSILON")
				{
					//insert the production in table[non_term , a]
					table[non_term][a].insert(p);
				}

				//handle epsilon
				else
				{
					set<string> nt_follow = follow(non_term);
					//for each terminal in follow(non_term) - here $ will also be returned in set
					// the mapping completes for $ as well , no need to include it separately
					for ( set<string>:: iterator iter_term = nt_follow.begin() ; iter_term != nt_follow.end() ; iter_term++ )
					{
						//insert prod in table[non_term , iter_term]
						table[ non_term ][(*iter_term)].insert(p);
					}
				}
			}

		}





	}

	void print_ll1_table()
	{
		for(Table::iterator iter_tab = table.begin() ; iter_tab != table.end() ; iter_tab++)
		{
			cout << "NT: " << (*iter_tab).first << endl;
			for(Row::iterator iter_row = (*iter_tab).second.begin() ; iter_row != (*iter_tab).second.end() ; iter_row++)
			{
				cout << "    T:" << (*iter_row).first << endl;
				for( TableEntry::iterator iter_entry = (*iter_row).second.begin() ; iter_entry !=(*iter_row).second.end() ; iter_entry++ )
				{
					cout << "       " ;
					print_production(*iter_entry);
				}
			}

			cout << endl << endl;
		}
	}


};

int main()
{



	Grammar grm;

	/*
	 * creating grammar:
	 * A -> BC|k
	 * B -> c | EPSILON
	 * C -> e | EPSILON
	 */
	grm.add_non_terminal("A");
	grm.add_non_terminal("B");
	grm.add_non_terminal("C");
	grm.add_terminal("a");
	grm.add_terminal("c");
	grm.add_terminal("e");
	grm.add_terminal("k");

	// A -> BC
	ProductionLeft pl = "A";
	ProductionRight pr;
	pr.push_back("B");
	pr.push_back("C");

	grm.add_production(pl , pr);

	 // A -> k
	pl = "A";
	pr.clear();
	pr.push_back("k");
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
	//grm.print_follow("B");



	//call this function to create/update the table of grammar after making changes.
	grm.update_table();
	
	
	
	//print the ll1 table associated
	grm.print_ll1_table();

}
