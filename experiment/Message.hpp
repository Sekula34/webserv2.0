
#ifndef MESSAGE_HPP
# define MESSAGE_HPP

# include <iostream>
# include <list>

class Node;

class Message {

	public:
									// canonical
									Message(void);
		virtual						~Message(void);
		void						bufferToNodes(char* buffer, size_t num);
		void						printChain();

									// set and get

									//Message specific functions
	private:
		std::list<Node> 			_chain;	
		std::list<Node>::iterator	_it;	
		bool						_chunked;
		bool						_trailer;

		void						_isNodeComplete();
		std::string					_delimiter(std::string & str);
		void						_parseNode();
		void						_addNewNode();

									Message(Message const & src);
		Message &					operator=(Message const & rhs);
};

#endif

