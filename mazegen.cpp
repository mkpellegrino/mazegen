//#define MAXSIZE 16

#include <iostream>
#include <vector>

#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>
int MAXSIZE;

bool isstart;
using namespace std;

class Node
{
public:
  Node();
  ~Node();
  Node( int a, int b );
  void setX( int a ){ x = a; };
  void setY( int a ){ y = a; };
  int getX(){ return x; };
  int getY(){ return y; };
  bool isValid(){ return validity; };
  void setWall();
  int number_of_connections();
  bool isConnected();
  bool addConnection( Node * n );
  bool removeConnection( Node * n );
  friend ostream &operator << (ostream &out, const Node &n);
private:
  vector<Node*> connections;
  int x;
  int y;
  bool wall;
  bool validity;
};

int Node::number_of_connections()
{
  return connections.size();
}

void Node::setWall()
{
  wall=true;
}
ostream & operator << (ostream &out, const Node &n) 
{
  //out << "Node: (" << n.x << "," << n.y << ")";
  if( n.connections.size() > 0 && (n.x>=0) && (n.y>=0) )
    {
      //out << "->" << endl;
      for( int i=0; i<n.connections.size(); i++ )
	{
	  // this would create an infinite recursive loop
	  //out << "\t" << *n.connections[i] << endl;
	  if( (n.connections[i]->getX()>=0) && (n.connections[i]->getY() >=0 ) )
	    {
	      out << "<line"
		  << " x1=\"" << 10 + n.x*30  << "\""
		  << " y1=\"" << 10 + n.y*30  << "\""
		  << " x2=\"" << 10 + n.connections[i]->getX()*30  << "\""
		  << " y2=\"" << 10 + n.connections[i]->getY()*30 << "\""
		  << " stroke-width=\"2\" stroke=\"#000000\"/>";
	    }
	  //out << "\tNode: (" << n.connections[i]->getX() << "," << n.connections[i]->getY() << ")" << endl;
	}
    }
  return out;
}
bool Node::isConnected()
{
  if( connections.size() == 0 ) return false;
  return true;
}

Node::Node()
{
#ifdef DEBUG2
  cout << "<!-- creating empty node -->" << endl;
#endif
  wall=false;
  validity=false;
}

Node::~Node()
{
#ifdef DEBUG2
  cout << "<!-- deleting node: (" << getX() << "," << getY() << ") -->" << endl;
#endif
}

  
  Node::Node( int a, int b )
  {
#ifdef DEBUG2
    cout << "<!-- creating node: (" << a << "," << b << ") -->" << endl;
#endif
    x=a;
    y=b;
    wall=false;
    if( a==-1 || b==-1 )
      {
	validity=false;
      }
    else
      {
	validity=true;
      }
  }

bool Node::removeConnection(Node * n)
{
#ifdef DEBUG2
  cout << "<!-- trying to remove connection: (" << n->getX() << "," << n->getY() << ") from (" << getX() << "," << getY() << ") -->" << endl;
#endif
  // Make sure we're not trying to remove a connection to ourself
  if( (n->getX() == x && n->getY() == y) || ( !n->isConnected() )) return false;

  for( int i=0; i<connections.size(); i++ )
    {
      if( connections.at(i) == n )
	{

	  connections.erase(connections.begin()+i);
	  n->removeConnection(this);
	  return true;
	  // no need to keep looking so
	  // break out of the loop
	}
    }
  return false;


}
bool Node::addConnection( Node * n )
{
#ifdef DEBUG2
  cout << "<!-- trying to add connection to (" << getX() << "," << getY() << ") -->" << endl;
#endif

  if( !n->isValid() )
    {
#ifdef DEBUG
      cout << "<!-- * * * INVALID NODE * * * -->" << endl << endl;
#endif
      return false;
    }
  // Make sure we're not trying to create a connection to self
  if( n->getX() == x && n->getY() == y ) return false;
  
  // create a connection from this Node to another
  // first, look through the connections, and make sure
  // n is not already in it.
  int s=connections.size();
  bool found=false;
  for( int i=0; i<s; i++ )
    {
      if( connections.at(i) == n )
	{
	  // found it in the connections list already
	  found = true;
	  // no need to keep looking so
	  // break out of the loop
	  i=s+1;
	}
    }
  
  if( found )
    {
#ifdef DEBUG
      cout << "<!-- * * * f a i l e d : already found in connections list -->" << endl;
#endif
      return( false );
    }
      
  // now add n to the connectioins
  connections.push_back(n);
  
  n->addConnection(this);
  
  return true;
}

class Field
{
public:
  Field()
  {
#ifdef DEBUG2    
    cout << "<!-- creating field -->" << endl;
#endif
  };
  ~Field();
  Field( int a, int b );
  
  Node * getNode(int i, int j);
private:
  vector<vector<Node*> > nodes;
  // size of field
  // width
  int x;
  // height
  int y;
  // depth
  int z;
};

Node * Field::getNode(int i, int j)
{
  if( i < 0 || j < 0 || i > x || j > y ) return new Node(-1,-1);
#ifdef DEBUG2
  cout << "<!-- field::getNode(" << i << "," << j << ") -->" << endl;
#endif  
  return nodes[i][j];
}

Field::~Field()
{
#ifdef DEBUG2
  cout << "<!-- deleting field -->" << endl;
#endif

}


Field::Field(int a, int b)
{
#ifdef DEBUG2
  cout << "<!-- creating field of size: " << a << " x " << b << "-->" << endl;
#endif
  x=a;
  y=b;
  for( int i=0; i<x; i++ )
    {
      vector<Node*> tmp;
      for( int j=0; j<y; j++ )
	{
	  tmp.push_back( new Node( i, j )  );
	}
      nodes.push_back(tmp);
    }
}


bool addRandomWall( Node * starting_node, Field * f)
{
#ifdef DEBUG
  cout << "<!-- addRandomWall() starting with: (" << starting_node->getX() << "," << starting_node->getY() << ") -->" << endl;
#endif
  int my_x = starting_node->getX();
  int my_y = starting_node->getY();
  
  if( my_x==-1 || my_y == -1 || my_x > MAXSIZE-2 || my_y > MAXSIZE-2) return false;
  vector <Node*> possibleNodes;
  
  if( !f->getNode( my_x, my_y+1 )->isConnected() && f->getNode( my_x, my_y+1 )->isValid() ) possibleNodes.push_back( f->getNode( my_x, my_y+1 ) );
  if( !f->getNode( my_x, my_y-1 )->isConnected() && f->getNode( my_x, my_y-1 )->isValid() ) possibleNodes.push_back( f->getNode( my_x, my_y-1 ) );
  if( !f->getNode( my_x+1, my_y )->isConnected() && f->getNode( my_x+1, my_y )->isValid() ) possibleNodes.push_back( f->getNode( my_x+1, my_y ) );
  if( !f->getNode( my_x-1, my_y )->isConnected() && f->getNode( my_x-1, my_y )->isValid() ) possibleNodes.push_back( f->getNode( my_x-1, my_y ) );

  if( possibleNodes.size() == 0 ) return false;

  int NSEW = rand() % possibleNodes.size();
#ifdef DEBUG
  cout << "<!--        (" << my_x << "," << my_y << ") Choosing: (" << possibleNodes[NSEW]->getX() << "," << possibleNodes[NSEW]->getY() << ") -->" << endl;
#endif
  // if possibleNodes[NESW] is already connected, don't do it and return false
  if( possibleNodes[NSEW]->isConnected() ) return false;
  
  starting_node->addConnection( possibleNodes[NSEW] );
  addRandomWall( possibleNodes[NSEW], f );
#ifdef DEBUG
  cout << "<!-- Done adding Random Wall -->" << endl;
#endif
  //starting_node->addConnection( possibleNodes[NSEW] );
  
  return true;

}
int main()
{
  cerr << "size? ";
  scanf( "%d", &MAXSIZE );
  isstart=true;
  srand (time(NULL));

  cout << "<html><svg width=\"" << MAXSIZE*30+10<< "px\" height=\"" << MAXSIZE*30+10 << "px\" version=\"1.1\"><defs></defs>" << endl;
  cout << "<text x=\"0\" y=\"30\">Start</text>" << endl;
  cout << "<text x=\"" << (MAXSIZE-1.5)*30+10 << "\" y=\""  << (MAXSIZE-1)*30 << "\">End</text>" << endl;

  Field * f = new Field( MAXSIZE, MAXSIZE );


  // This creates the border
  
  for( int i=0; i<MAXSIZE-1; i++ )
    {
      
      f->getNode(0,i)->addConnection( f->getNode(0,i+1) ); 
      f->getNode(MAXSIZE-1,i)->addConnection( f->getNode(MAXSIZE-1,i+1) );
      f->getNode(i,0)->addConnection( f->getNode(i+1,0) );
      f->getNode(i,MAXSIZE-1)->addConnection( f->getNode(i+1,MAXSIZE-1) );
      
    }



#ifdef DEBUG
  cout << endl << endl << "<!-- Initial Walls -->" << endl;

#endif
  int random_part_of_a_wall;
  bool supress=false;
  for( int i=0; i< 10; i++ )
    {
      //random_part_of_a_wall = (rand() % (MAXSIZE/4))+MAXSIZE/2 ;
      if( !supress && f->getNode(random_part_of_a_wall,1)->number_of_connections() <= 2 )
	{
	  random_part_of_a_wall = (rand() % (MAXSIZE));
	  // North Border
	  //f->getNode(random_part_of_a_wall,1)->addConnection( f->getNode(random_part_of_a_wall,0) );
	  //addRandomWall( f->getNode(random_part_of_a_wall,1), f );
	  //f->getNode(random_part_of_a_wall,1)->addConnection( f->getNode(random_part_of_a_wall,0) );
	  addRandomWall( f->getNode(random_part_of_a_wall,0), f );
#ifdef DEBUG
	  cout << "<text x=\"" << (random_part_of_a_wall)*30+10 << "\" y=\""  << 30 << "\">*</text>" << endl;
#endif
	}

      

      if( !supress && f->getNode(1,random_part_of_a_wall)->number_of_connections() <= 2 )
	{
	  // West Border
	  //random_part_of_a_wall = (rand() % (MAXSIZE/4))+MAXSIZE/2 ;
	  random_part_of_a_wall = (rand() % (MAXSIZE));
	  //f->getNode(1,random_part_of_a_wall)->addConnection( f->getNode(0,random_part_of_a_wall) );
	  //addRandomWall( f->getNode(1,random_part_of_a_wall), f );
	  //f->getNode(1,random_part_of_a_wall)->addConnection( f->getNode(0,random_part_of_a_wall) );
	  addRandomWall( f->getNode(0,random_part_of_a_wall), f );
#ifdef DEBUG
	  cout << "<text x=\"" << 10 << "\" y=\""  << (random_part_of_a_wall)*30+10 << "\">*</text>" << endl;
#endif
	}
      

      if( !supress && f->getNode(random_part_of_a_wall,MAXSIZE-1)->number_of_connections() <= 2 )
	{
	  
	  // South Border
	  //random_part_of_a_wall = (rand() % (MAXSIZE/4))+MAXSIZE/2 ;
	  random_part_of_a_wall = (rand() % (MAXSIZE));
	  //f->getNode(random_part_of_a_wall,MAXSIZE-2)->addConnection( f->getNode(random_part_of_a_wall,MAXSIZE-1) );
	  //addRandomWall( f->getNode(random_part_of_a_wall,MAXSIZE-2), f );
	  //f->getNode(random_part_of_a_wall,MAXSIZE-2)->addConnection( f->getNode(random_part_of_a_wall,MAXSIZE-1) );
	  addRandomWall( f->getNode(random_part_of_a_wall,MAXSIZE-1), f );
#ifdef DEBUG
     	  cout << "<text x=\"" << (random_part_of_a_wall)*30+10 << "\" y=\""  << MAXSIZE*30+10 << "\">*</text>" << endl;
#endif	  
	}
      
      if( !supress && f->getNode(MAXSIZE-1,random_part_of_a_wall)->number_of_connections() <= 2 )
	{
	  // East Border
	  //random_part_of_a_wall = (rand() % (MAXSIZE/4))+MAXSIZE/2 ;
	  random_part_of_a_wall = (rand() % (MAXSIZE));
	  //f->getNode(MAXSIZE-2,random_part_of_a_wall)->addConnection( f->getNode(MAXSIZE-1,random_part_of_a_wall) );
	  //addRandomWall( f->getNode(MAXSIZE-2,random_part_of_a_wall), f );
	  //f->getNode(MAXSIZE-2,random_part_of_a_wall)->addConnection( f->getNode(MAXSIZE-1,random_part_of_a_wall) );
	  addRandomWall( f->getNode(MAXSIZE-1,random_part_of_a_wall), f );
#ifdef DEBUG
	  cout << "<text x=\"" << (MAXSIZE-1)*30 << "\" y=\""  << (random_part_of_a_wall)*30+10 << "\">*</text>" << endl;
#endif
	}
    }
 

  for( int i=0; i<MAXSIZE; i++ )
    {
      for( int j=0; j<MAXSIZE; j++ )
	{
	  if( !supress ) addRandomWall( f->getNode(i,j), f );
	}
    }


  
  // Remove the top left and bottom right
  // top left
  f->getNode(0,0)->removeConnection( f->getNode(0,1) );
  f->getNode(0,1)->removeConnection( f->getNode(0,0) );

  // bottom right
  
  f->getNode(MAXSIZE-1,MAXSIZE-2)->removeConnection( f->getNode(MAXSIZE-1,MAXSIZE-1) );
  
  f->getNode(MAXSIZE-1,MAXSIZE-1)->removeConnection( f->getNode(MAXSIZE-1,MAXSIZE-2) );

  // addRandomWall( f->getNode(5,4), f );

  /* DISPLAY THE MAZE */
  for( int i=0; i<MAXSIZE; i++ )
    {
      for( int j=0; j<MAXSIZE; j++ )
	{
	  cout << *f->getNode(i,j) << endl;
	}

    }

  cout << "</svg></html>" << endl;
  delete f;
  return 0;
}


