
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
		void			addStr(char* buffer);

						// set and get

						//Message specific functions
	private:
		std::list<Node> _chain;	
		std::list<Node>::iterator _it;	
		void			_checkNodeComplete();
		std::string		_delimiter(std::string & str);
						Message(Message const & src);
		Message &		operator=(Message const & rhs);
};

#endif

