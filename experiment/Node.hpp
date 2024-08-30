
#ifndef NODE_HPP
# define NODE_HPP

# define	HEADER	0
# define	BODY	1
# define	CHUNK	2
# define	TRAIL	3

# include <iostream>

class Node {

	public:
							// canonical
							Node (const std::string & str, int type);
							~Node(void);

							// set and get
		void				setString(const std::string & s);
		std::string			getStringUnchunked();
		std::string			getStringChunked();

							//Node specific functions
		std::string			_chunk();
		std::string			_unChunk();

	private:
		int					_state;
		const int			_type;
		const std::string 	_str;
							Node(void);
							Node(Node const & src);
		Node &				operator=(Node const & rhs);
};

#endif

