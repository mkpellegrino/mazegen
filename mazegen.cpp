#define MAXSIZE 36

#include <iostream>
#include <vector>

#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>

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

  void setWall();
  
  bool isConnected();
  bool addConnection( Node * n );
  bool removeConnection( Node * n );
  friend ostream &operator << (ostream &out, const Node &n);
private:
  vector<Node*> connections;
  int x;
  int y;
  bool wall;
};
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
#ifdef DEBUG
    cerr << "creating node" << endl;
#endif
    wall=false;
  }

Node::~Node()
  {
#ifdef DEBUG
    cerr << "deleting node" << endl;
#endif
  }


Node::Node( int a, int b )
  {
#ifdef DEBUG
    cerr << "creating node: (" << a << "," << b << ")" << endl;
#endif
    x=a;
    y=b;
    wall=false;
  }

bool Node::removeConnection(Node * n)
{
#ifdef DEBUG
  cerr << "trying to remove connection: " << n->getX() << "," << n->getY() << endl << "from " << getX() << "," << getY() << endl;
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
#ifdef DEBUG
  cerr << "trying to add connection " << endl;
#endif

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
      cerr << " * * * f a i l e d : already found in connections list" << endl;
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
#ifdef DEBUG    
    cerr << "creating field" << endl;
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
#ifdef DEBUG
  cerr << "field::getNode(" << i << "," << j << ")" << endl;
#endif  
  return nodes[i][j];
}

Field::~Field()
{
#ifdef DEBUG
  cerr << "deleting field" << endl;
#endif

}


Field::Field(int a, int b)
{
#ifdef DEBUG
  cerr << "creating field of size: " << a << " x " << b << endl;
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
  cerr << "addRandomWall()" << endl;
  
  int my_x = starting_node->getX();
  int my_y = starting_node->getY();
  
  if( my_x==-1 || my_y == -1 || my_x > MAXSIZE-2 || my_y > MAXSIZE-2) return false;
  vector <Node*> possibleNodes;
  
  if( !f->getNode( my_x, my_y+1 )->isConnected()) possibleNodes.push_back( f->getNode( my_x, my_y+1 ) );
  if( !f->getNode( my_x, my_y-1 )->isConnected()) possibleNodes.push_back( f->getNode( my_x, my_y-1 ) );
  if( !f->getNode( my_x+1, my_y )->isConnected()) possibleNodes.push_back( f->getNode( my_x+1, my_y ) );
  if( !f->getNode( my_x-1, my_y )->isConnected()) possibleNodes.push_back( f->getNode( my_x-1, my_y ) );

  if( possibleNodes.size() == 0 ) return false;

  int NSEW = rand() % possibleNodes.size();

  cerr << "(" << my_x << "," << my_y << ") Choosing: (" << possibleNodes[NSEW]->getX() << "," << possibleNodes[NSEW]->getY() << ")" << endl;
  starting_node->addConnection( possibleNodes[NSEW] );
  addRandomWall( possibleNodes[NSEW], f );

  cerr << "Done adding Random Wall" << endl;

  //starting_node->addConnection( possibleNodes[NSEW] );
  
  return true;

}
int main()
{
  isstart=true;
  srand (time(NULL));

  cout << "<html><svg width=\"1080px\" height=\"1080px\" version=\"1.1\"><defs></defs>" << endl;
  cout << "<text x=\"0\" y=\"30\">Start</text>" << endl;
  cout << "<text x=\"" << (MAXSIZE-1.5)*30+10 << "\" y=\""  << (MAXSIZE-1)*30 << "\">End</text>" << endl;

  Field * f = new Field( MAXSIZE, MAXSIZE );

  for( int i=0; i<MAXSIZE-1; i++ )
    {
      
      f->getNode(0,i)->addConnection( f->getNode(0,i+1) ); 
      f->getNode(MAXSIZE-1,i)->addConnection( f->getNode(MAXSIZE-1,i+1) );
      f->getNode(i,0)->addConnection( f->getNode(i+1,0) );
      f->getNode(i,MAXSIZE-1)->addConnection( f->getNode(i+1,MAXSIZE-1) );
      
    }

  int random_part_of_a_wall = (rand() % (MAXSIZE/4))+MAXSIZE/2 ;
  // North Border
  f->getNode(random_part_of_a_wall,1)->addConnection( f->getNode(random_part_of_a_wall,0) );
  addRandomWall( f->getNode(random_part_of_a_wall,1), f );

  
  // West Border
  random_part_of_a_wall = (rand() % (MAXSIZE/4))+MAXSIZE/2 ;
  f->getNode(1,random_part_of_a_wall)->addConnection( f->getNode(0,random_part_of_a_wall) );
  addRandomWall( f->getNode(1,random_part_of_a_wall), f );

  // TODO: Why do the Southern and Eastern Borders sometimes connect creating a non-maze?

  // South Border
  random_part_of_a_wall = (rand() % (MAXSIZE/4))+MAXSIZE/2 ;
  f->getNode(random_part_of_a_wall,MAXSIZE-2)->addConnection( f->getNode(random_part_of_a_wall,MAXSIZE-1) );
  addRandomWall( f->getNode(random_part_of_a_wall,MAXSIZE-2), f );

  // East Border
  random_part_of_a_wall = (rand() % (MAXSIZE/4))+MAXSIZE/2 ;
  f->getNode(MAXSIZE-2,random_part_of_a_wall)->addConnection( f->getNode(MAXSIZE-1,random_part_of_a_wall) );
  addRandomWall( f->getNode(MAXSIZE-2,random_part_of_a_wall), f );
  
 

  for( int i=0; i<MAXSIZE; i++ )
    {
      for( int j=0; j<MAXSIZE; j++ )
	{
	  addRandomWall( f->getNode(i,j), f );
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


