
#ifndef MESSAGE_HPP
# define MESSAGE_HPP

# include <iostream>
# include <list>

class Node;

class Message {

	public:
						// canonical
						Message(void);
		virtual			~Message(void);

						// set and get

						//Message specific functions
	private:
		std::list<Node> _chain;	
						Message(Message const & src);
		Message &		operator=(Message const & rhs);
};

std::ostream &			operator<<(std::ostream & o, Message const & rhs);
#endif

