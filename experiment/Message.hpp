
#ifndef MESSAGE_HPP
# define MESSAGE_HPP

# include <iostream>
# include <list>
# include <sstream>

class Node;

class Message {

	public:
									// canonical
									Message(void);
		virtual						~Message(void);
		void						bufferToNodes(char* buffer, size_t num);
		void						printChain();

									// set and get
		int							getState() const;

									//Message specific functions
	private:
		std::list<Node> 			_chain;	 // linked list of nodes
		std::list<Node>::iterator	_it;	// pointing to current node in chain
		bool						_chunked; // this is a chunked request
		bool						_trailer; // we are expecting a trailer as last node
		int							_state; // message is complete
		std::stringstream 			_ss;

		void						_isNodeComplete();
		void						_parseNode();
		void						_addNewNode();
		size_t						_calcChunkSize(std::string s);

									Message(Message const & src);
		Message &					operator=(Message const & rhs);
};

#endif

