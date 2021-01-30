// (C) 2021 - Michael K. Pellegrino
// A maze generator

#include <iostream>
#include <vector>

#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>

int HEIGHT;
int WIDTH;
bool signature=false;
bool latex=true;
bool game=false;
int uniqueid=0;
using namespace std;



class LineValue
{
public:
  LineValue();
  ~LineValue();
  LineValue( int a, int b, int c, int d );
  bool same_as( int a, int b, int c, int d );
private:
  int A;
  int B;
  int C;
  int D;
};

bool LineValue::same_as( int a, int b, int c, int d )
{
  if( A==a && B==b && C==c && D==d ) return true;
  return false;
}

LineValue::LineValue( int a, int b, int c, int d )
{
  A=a;B=b;C=c;D=d;
}


vector<LineValue*> redundancy_list;

// a global function to check to see if
// a line segment has already been 'outed'
bool isDuplicate(int a, int b, int c, int d )
{
  for( int i=0; i < redundancy_list.size(); i++ )
    {
      if( redundancy_list[i]->same_as( a, b, c, d ) )
	{
#ifdef DEBUG
	  cerr << "found duplicate" << endl;
#endif
	  return true;
	}
    }
  LineValue * tmp = new LineValue( a, b, c, d );
  redundancy_list.push_back( tmp );
  return false;
}

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
  int number_of_connections();
  bool isConnected();
  bool addConnection( Node * n );
  bool removeConnection( Node * n );
  friend ostream &operator << (ostream &out, const Node &n);
private:
  vector<Node*> connections;
  int x;
  int y;
  bool validity;
};

int Node::number_of_connections()
{
  return connections.size();
}

ostream & operator << (ostream &out, const Node &n) 
{
  if( n.connections.size() > 0 && (n.x>=0) && (n.y>=0) )
    {
      for( int i=0; i<n.connections.size(); i++ )
	{
	  // this would create an infinite recursive loop
	  if( (n.connections[i]->getX()>=0) && (n.connections[i]->getY() >=0 ) )
	    {
	      int A;
	      int B;
	      int C;
	      int D;
	      if( n.x <= n.connections[i]->getX() )
		{
		  A=n.x;
		  B=n.y;
		  C=n.connections[i]->getX();
		  D=n.connections[i]->getY();
		}
	      else
		{
		  A=n.connections[i]->getX();		      
		  B=n.connections[i]->getY();		      
		  C=n.x;
		  D=n.y;
		}

	      if( latex )
		{
		  if( !isDuplicate( A, B, C, D ) )
		  out << "\\draw[thick,-] ("
		      << 5 + A  << "," << -1*(5 + B)  << ") -- ("
		      << 5 + C  << "," << -1*(5 + D) << ");";
		}
	      else
		{
		  if( !isDuplicate( A, B, C, D ) )
		    out << "<line"
			<< " id=\"wall." << uniqueid++ << "\""
			<< " x1=\"" << 10 + A*30  << "\""
			<< " y1=\"" << 10 + B*30  << "\""
			<< " x2=\"" << 10 + C*30  << "\""
			<< " y2=\"" << 10 + D*30  << "\""
			<< " stroke-width=\"2\" stroke=\"#000000\"/>";
		}
	    }
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
  
  if( my_x==-1 || my_y == -1 || my_x > WIDTH-2 || my_y > HEIGHT-2) return false;
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
  cerr << "1) LaTeX\t2) svg/html\t3) interactive svg/html:";
  scanf( "%d", &HEIGHT );
  if( HEIGHT==2 )
    latex=false;
  if( HEIGHT==3 )
    {
      latex=false;
      game=true;
    }

  cerr << "recommended maximum height and width is 122" << endl;
  cerr << "height? ";
  scanf( "%d", &HEIGHT );

  cerr << "width? ";
  scanf( "%d", &WIDTH );

  WIDTH++; // account for the missing column
  HEIGHT++; // account for the missing row
  
  srand (time(NULL));

  if( latex )
    {
      double xscale = (double)(int(100*7.5/(0.41*WIDTH)))/100;
      double yscale = (double)(int(100*10/(0.41*HEIGHT)))/100;
      cout << "\\documentclass[12pt]{article}" << endl
	   << "\\usepackage[letterpaper,total={7.5in,9.5in},top=0.75in]{geometry}" << endl 
	   << "\\usepackage{tikz}" << endl
	   << "\\begin{document}" << endl
	   << "\\thispagestyle{empty}" << endl
	   << "\\begin{center}" << endl

	   << "\\begin{tikzpicture}[xscale=" << xscale << ",yscale=" << yscale << "]" << endl;
      
      
    }
  else
    {
      cout << "<html>" << endl;
      cout << "<head><title>A Maze Game - (C) 2021 - Michael K. Pellegrino</title>" << endl;
      if( game )
	{
	  cout << "<script language=\"javascript\">var things=[];document.addEventListener('keydown', Kdown);function Kdown(e){var elm=document.getElementById(\"btn\");var x=elm.getAttribute('cx');var y=elm.getAttribute('cy');var K=e.keyCode;if(K==38||K==73){elm.setAttribute('cy',(1)*(y)-5);if(collision()==1)elm.setAttribute('cy',(1)*(y)+5);}else if(K==37||K==74){elm.setAttribute('cx',(1)*(x)-5);if(collision()==1)elm.setAttribute('cx',(1)*(x)+5);}else if(K==40||K==75){elm.setAttribute('cy',(1)*(y)+5);if(collision()==1)elm.setAttribute('cy',(1)*(y)-5);}else if(K==39||K==76){elm.setAttribute('cx',(1)*(x)+5);if(collision()==1)elm.setAttribute('cx',(1)*(x)-5);}if(winner()==1){alert(\"YOU'VE MADE IT!\");}}function onload(){var k=0;var elm;var duplicate=0;for(i=0; i<1920; i++){x=\"wall.\" + i;elm=document.getElementById(x);if(elm !=null)things[k++]=elm;}start_time=new Date();}function winner(){var s1=document.getElementById(\"btn\");var x=1*s1.getAttribute('cx');var y=1*s1.getAttribute('cy');if(x > 895 && y > 890)return 1;return 0;}function collision(){var s1=document.getElementById(\"btn\");s1x=1*s1.getAttribute('cx');s1y=1*s1.getAttribute('cy');var s2;for(i=0; i<things.length; i++){s2=things[i];var s2x1=1*s2.getAttribute('x1');var s2x2=1*s2.getAttribute('x2');var s2y1=1*s2.getAttribute('y1');var s2y2=1*s2.getAttribute('y2');var TLx;var TLy;var BRx;var BRy;if(s2x1==s2x2){TLx=1*s2x1-4;BRx=1*s2x2+4}else if(s2x1<s2x2){TLx=s2x1;BRx=s2x2;}else {TLx=s2x2;BRx=s2x1;}if(s2y1==s2y2){TLy=1*s2y1-4;BRy=1*s2y2+4;}else if(s2y1<s2y2){TLy=s2y1;BRy=s2y2;}else{TLy=s2y2;BRy=s2y1;}if((s1x <=BRx)&&(s1x >=TLx)&&(s1y <=BRy)&&(s1y >=TLy))return 1;}return 0;}</script></head><body onload=\"onload();\"><center>" << endl;
	}
      else
	{
	  cout << "</head>" << endl;
	}      

      cout << "<center>A Maze Amusement - Use Arrow Keys or I (up), K (down), J (left), or L (right)<br>" << endl;
      cout << "<svg width=\"" << 500 << "px\" height=\"" << 500 << "px\" version=\"1.1\" viewBox=\"" << 0 << " " << 0 << " " << WIDTH*30+10 << " " << HEIGHT*30+10 << "\"><defs></defs>" << endl;
      cout << "<circle id=\"btn\" r=\"5\" cx=\"25\" cy=\"25\" fill=\"red\"/>" << endl;
      cout << "<text x=\"0\" y=\"30\">Start</text>" << endl;
      cout << "<text x=\"" << (WIDTH-1.5)*30+10 << "\" y=\""  << (HEIGHT-1)*30 << "\">End</text>" << endl;
    }
  Field * f = new Field( WIDTH, HEIGHT );


  // This creates the border
  
  for( int i=0; i<WIDTH-1; i++ )
    {
      f->getNode(i,0)->addConnection( f->getNode(i+1,0) );
      f->getNode(i,HEIGHT-1)->addConnection( f->getNode(i+1,HEIGHT-1) ); 
    }
  for( int i=0; i<HEIGHT-1; i++ )
    {
      f->getNode(0,i)->addConnection( f->getNode(0,i+1) ); 
      f->getNode(WIDTH-1,i)->addConnection( f->getNode(WIDTH-1,i+1) );
    }


  // create signature
  if( (HEIGHT > 10) && (WIDTH>15) && signature )
  {
    f->getNode(7,8)->addConnection( f->getNode(7,7) );
    f->getNode(7,7)->addConnection( f->getNode(7,6) );
    f->getNode(7,6)->addConnection( f->getNode(8,6) );
    f->getNode(8,6)->addConnection( f->getNode(8,7) );
    f->getNode(8,6)->addConnection( f->getNode(9,6) );
    f->getNode(9,6)->addConnection( f->getNode(9,7) );
    f->getNode(9,7)->addConnection( f->getNode(9,8) );
    f->getNode(9,8)->addConnection( f->getNode(9,9) );
    f->getNode(10,8)->addConnection( f->getNode(10,7) );
    f->getNode(10,7)->addConnection( f->getNode(10,6) );
    f->getNode(10,6)->addConnection( f->getNode(11,6) );
    f->getNode(11,6)->addConnection( f->getNode(11,7) );
    f->getNode(11,7)->addConnection( f->getNode(10,7) );
    //f->getNode(,)->addConnection( f->getNode(,) );
    //f->getNode(,)->addConnection( f->getNode(,) );
    //f->getNode(,)->addConnection( f->getNode(,) );
    //f->getNode(,)->addConnection( f->getNode(,) );
    //f->getNode(,)->addConnection( f->getNode(,) );

  }



#ifdef DEBUG
  cout << endl << endl << "<!-- Initial Walls -->" << endl;
#endif
  int random_part_of_a_wall;
  bool supress=false;

  int number_of_side_walls = HEIGHT/2;
  int number_of_top_walls = WIDTH/2;
  for( int i=0; i< number_of_side_walls; i++ )
    {
      // Western Border
      random_part_of_a_wall = rand() % HEIGHT;
      if( !supress && f->getNode(1,random_part_of_a_wall)->number_of_connections() <= 2 )
	{
	  addRandomWall( f->getNode(0,random_part_of_a_wall), f );
#ifdef DEBUG
	  cout << "<text x=\"" << 10 << "\" y=\""  << (random_part_of_a_wall)*30+10 << "\">*</text>" << endl;
#endif
	}

      // Eastern Border
      random_part_of_a_wall = rand() % HEIGHT;
      if( (!supress) &&
	  (f->getNode(WIDTH-1,random_part_of_a_wall)->number_of_connections() <= 2) &&
	  (f->getNode(WIDTH-2,random_part_of_a_wall)->number_of_connections() == 0) 
	  )
	{
	  f->getNode(WIDTH-2,random_part_of_a_wall)->addConnection( f->getNode(WIDTH-1,random_part_of_a_wall) );
	  addRandomWall( f->getNode(WIDTH-2,random_part_of_a_wall), f );
#ifdef DEBUG
	  cout << "<text x=\"" << (WIDTH-1)*30 << "\" y=\""  << (random_part_of_a_wall)*30+10 << "\">*</text>" << endl;
#endif
	}
    }

  for( int i=0; i< number_of_top_walls; i++ )
    {
      // Northern Border
      random_part_of_a_wall = rand() % WIDTH;
      if( !supress && f->getNode(random_part_of_a_wall,1)->number_of_connections() <= 2 )
	{
	  addRandomWall( f->getNode(random_part_of_a_wall,0), f );
#ifdef DEBUG
	  cout << "<text x=\"" << (random_part_of_a_wall)*30+10 << "\" y=\""  << 30 << "\">*</text>" << endl;
#endif
	}
      random_part_of_a_wall = rand() % WIDTH;
      if ( (!supress) &&
	   (f->getNode(random_part_of_a_wall,HEIGHT-1)->number_of_connections() <= 2) &&
	   (f->getNode(random_part_of_a_wall,HEIGHT-2)->number_of_connections() == 0) 
	   )
	{
	  f->getNode(random_part_of_a_wall,HEIGHT-1)->addConnection( f->getNode(random_part_of_a_wall,HEIGHT-2) );
	  addRandomWall( f->getNode(random_part_of_a_wall,HEIGHT-2), f );
#ifdef DEBUG
     	  cout << "<text x=\"" << (random_part_of_a_wall)*30+10 << "\" y=\""  << HEIGHT*30+10 << "\">*</text>" << endl;
#endif	  
	}


    }
  // *****


  // fill empty spots with walls
  for( int i=0; i<WIDTH; i++ )
    {
      for( int j=0; j<HEIGHT; j++ )
	{
	  if( !supress ) addRandomWall( f->getNode(i,j), f );
	}
    }


  
  // Remove the top left and bottom right
  // top left
  f->getNode(0,0)->removeConnection( f->getNode(0,1) );
  f->getNode(0,1)->removeConnection( f->getNode(0,0) );

  // bottom right
  
  f->getNode(WIDTH-1,HEIGHT-2)->removeConnection( f->getNode(WIDTH-1,HEIGHT-1) );
  f->getNode(WIDTH-1,HEIGHT-1)->removeConnection( f->getNode(WIDTH-1,HEIGHT-2) );
  
  
  /* DISPLAY THE MAZE */
  uniqueid=0;
  for( int i=0; i<WIDTH; i++ )
    {
      for( int j=0; j<HEIGHT; j++ )
	{
	  cout << *f->getNode(i,j) << endl;
	}
    }
  if( latex )
    {
      // add entrance and exit
      cout << "\\draw[thick,->] (5,-5.5) -- (5.25, -5.5);" << endl;
      cout << "\\draw[thick,->] ("<< (double)WIDTH+3.75 << "," << (double)(-1*HEIGHT)-3.5 << ") -- (" << (double)WIDTH+4 << "," << (double)(-1*HEIGHT)-3.5 << ");" << endl;
      cout << "\\end{tikzpicture}" << endl;
      cout << "\\par{} \\noindent \\tiny (C) 2021 - Michael K. Pellegrino - mkpelleg.freeshell.org" << endl;
      cout << "\\end{center}" << endl;
      cout << "\\end{document}" << endl;
    }
  else
    {
      cout << "</svg>" << endl;
      cout << "<p><font size=-2><i>(C) 2021 - Michael K. Pellegrino - <a target=_blank href=\"https://github.com/mkpellegrino/mazegen\">github.com/mkpellegrino/mazegen</a></i></font></p>" << endl;
      cout << "<p><font size=-2><i>mazegen 1.0 - 2021 01 29</i></font></p>" << endl; 
      cout << "</html>" << endl;
    }
  delete f;
  return 0;
}


