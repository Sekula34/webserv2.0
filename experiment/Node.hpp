
#ifndef NODE_HPP
# define NODE_HPP

# define	HEADER		0
# define	BODY		1
# define	CHUNK		2
# define	TRAIL		3

# define	INCOMPLETE	0
# define	COMPLETE	1
# define	ERROR		-1

# include <iostream>

class Node {

	public:
							// canonical
							Node (const std::string & str, int type);
							~Node(void);
							Node(void);
							Node(Node const & src);

							// set and get
		void				setString(const std::string & s);
		void				setState(int state);
		void				concatString(const std::string & s);
		std::string			getStringUnchunked();
		std::string			getStringChunked();
		const int &			getType() const;
		const int &			getState() const;

							//Node specific functions
		std::string			_chunk();
		std::string			_unChunk();

	private:
		int					_state;
		const int			_type;
		std::string 		_str;
		Node &				operator=(Node const & rhs);
};

#endif

