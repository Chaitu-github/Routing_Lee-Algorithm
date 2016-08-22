///Routing
///Sai Chaitanya Nandipati
///M08905912


#include <iostream>
#include<vector>
#include<fstream>
#include<deque>
#include<cmath>
#include<algorithm>
#include<cstdlib>
#include<ctime>
#include<math.h>

#define via_at_bend 1  //to represent the via
#define via_blockage 2//blockage around the via
#define cell 3 //to represent the cell
#define cell_blockage 4 //to represent the blockage around the cell
#define terminal1 5
#define terminal2 6
#define terminal3 7
#define terminal4 8
#define success 1
#define f_findpath -2
#define f_backtarce -3
#define f_clearance -4

#define row_spacing 60

#define column_spacing 60

#define hor 0
#define ver 1



using namespace std;



///*********coordinates*****///
struct coordinates
{
    int x,y;
};


///*********for storing the net_vector information********///
struct netinfo
{
	int s_cell,t_cell;
	int s_terminal,t_terminal;
	int priority;
	int net_number;
};
vector<netinfo> net_vector;
vector <netinfo> unrouted_nets;
vector <netinfo> unrouted_nets_2;
///********************************************///



///***********storing placement grid information*******************************///
struct placement_grid
{
    coordinates p_grid;
    int cell_number;
};

int RR=1,CC=1;
vector<vector<placement_grid> > p_matrix;
///*******************************************///




///*********Individual blocks of a p_matrix******///
struct grid
{
	int b[2];	int net_ref[2];
    int via;
	int LeeNumb;
	int xcoordinate,ycoordinate;
	int netnumb[2];
	int cellnumber;
	int via1;

   grid(){LeeNumb=-2;
          netnumb[hor]=-1;
          netnumb[ver]=-1;
		  net_ref[hor]=-1;
          net_ref[ver]=-1;
          cellnumber=-1;}
};
int l=2;
///********layout_matrix*****************///
int layout_RR=1;
int layout_CC=1;
vector <vector <grid> > layout_matrix;
///**************************************///

deque <grid> propagatelist;

int NumberOfNets;
int NumberOfCells;
int netNum,c1,t1,c2,t2;




///****  for source target coordinates****///
struct source_target_index
{
	int netnumb,hs,vs,ht,vt;
};
vector <source_target_index> source_target;
///*******************************************///


///*****struct for terminal indexes*****////
struct cell_terminals
{
	char flip_direction;
	coordinates start_index;
	coordinates t[4];

};

vector <cell_terminals> cell_terminal_vector;

///*****************************///
int numberofvias=0;
int finalwirelength=0;

///*****function declarations*****///
void InitializeNets();
void Initializing_layout_coordinates();
void PrintCell(char select,int h, int v,int ht=layout_RR,int vt=layout_CC);
void printdeque();

int findpath(int netnumb,int hs,int vs,int ht,int vt);
int backtrace(int netnumb,int hs,int vs,int ht,int vt);
int clearlee(int netnumb,int hs, int vs,int ht,int vt);

void Outputfile();
bool distance(int x1,int y1,int x2,int y2);
void routing(int);
bool cb(int h,int v);
void initializing_layout_matrix();
void clear_the_matrix();
void flip_the_cell(char present_direction,int h,int v,int cellnumb);
void initialize_cell_terminal_vector();
void initializing_placement_matrix();
void via_side_blockage(char choice,int h,int v);
bool sort_the_nets(const netinfo &i,const netinfo &j);

void via_side_blockage_clear(char choice,int h,int v);
int backtrace_clear(int netnumb,int hs,int vs,int ht,int vt);
bool sort_using_priority(netinfo i,netinfo j);
void sort_using_net_priority(vector <netinfo> temp_vector);
void block_cell_terminals(int h,int v,int cellnumber);

void clear_cell_terminals(int tnumb,int h,int v);
int manhattendistance=0;
char *inputfilename[3];
char *outputfilename[3];
char *placementinput[3];

int main(int argc,char * argv[])
{
inputfilename[1]=argv[1];
outputfilename[1]=argv[2];
placementinput[1]=argv[3];

InitializeNets();

RR=ceil(sqrt(NumberOfCells));
CC=RR;

p_matrix.resize(RR,vector<placement_grid>(CC));
l=2;

layout_RR=(6+row_spacing)*RR+l*row_spacing;
layout_CC=(6+column_spacing)*CC+l*column_spacing;
layout_matrix.resize(layout_RR,vector<grid> (layout_CC));

initializing_placement_matrix();
Initializing_layout_coordinates();
initializing_layout_matrix();
routing(0);
routing(1);
int netnumb,sc,st,tc,tt,hs,vs,ht,vt;
Outputfile();
cout<<"manhatten distance "<<manhattendistance<<endl;
cout<<"number of vias "<<numberofvias<<endl;
cout<<"Total wirelength "<<finalwirelength<<endl;
}

///**************Initializing layout matrix from placemnt matrix*************///
void initializing_layout_matrix()
{
//	cell_terminal_vector.resize(NumberOfCells);
cell_terminal_vector.resize(NumberOfCells);
int choice=1;
	int temp_h,temp_v,c_numb;
	for(int i=0;i<RR;++i)
	{
	   for (int j=0;j<CC;++j)
	   {

	     if(p_matrix.at(i).at(j).cell_number!=-1)
	     {
				if(choice ==1)

				{
						temp_h=6*i+row_spacing*(i+1);
						temp_v=6*j+column_spacing*(j+1);
				}
				else if(choice ==2)
				{
								if(i%2==0)
								{
								  temp_h=6*i+row_spacing*(i+1);
								  temp_v=6*j+column_spacing*(j+1);
								}
								else
								{
								  temp_h=6*i+row_spacing*(i+1);
								  temp_v=(6*j+column_spacing*(j+2))-3;
								}
				}
                c_numb=p_matrix.at(i).at(j).cell_number;
				cell_terminal_vector.at(c_numb).start_index.x=temp_h;
				cell_terminal_vector.at(c_numb).start_index.y=temp_v;
				cell_terminal_vector.at(c_numb).t[0].x=temp_h;
				cell_terminal_vector.at(c_numb).t[0].y=temp_v+1;
				cell_terminal_vector.at(c_numb).t[1].x=temp_h;
				cell_terminal_vector.at(c_numb).t[1].y=temp_v+4;
				cell_terminal_vector.at(c_numb).t[2].x=temp_h+5;
				cell_terminal_vector.at(c_numb).t[2].y=temp_v+1;
				cell_terminal_vector.at(c_numb).t[3].x=temp_h+5;
				cell_terminal_vector.at(c_numb).t[3].y=temp_v+4;

           for(int m=temp_h-1;m<temp_h+7;++m)
			{
				for(int n=temp_v-1;n<temp_v+7;++n)
				{
                    if(m==temp_h+2 && n==temp_v+2)
					{layout_matrix.at(m).at(n).cellnumber=p_matrix.at(i).at(j).cell_number;}

                    if (m==temp_h-1 || m==temp_h+6 || n==temp_v-1 || n==temp_v+6)
					 {layout_matrix.at(m).at(n).via=cell_blockage;}
					else
					 {layout_matrix.at(m).at(n).via=cell;}
			    }
			}
			layout_matrix[temp_h-1][temp_v+1].via=0;///terminal 1 upper blockage clear
			layout_matrix[temp_h][temp_v+1].via=terminal1;///terminal 1 blockage clear
			layout_matrix[temp_h-1][temp_v+4].via=0;///terminal 2 upper blockage clear
			layout_matrix[temp_h][temp_v+4].via=terminal2;///terminal 2 blockage clear
			layout_matrix[temp_h+6][temp_v+1].via=0;///terminal 3 down blockage clear
			layout_matrix[temp_h+5][temp_v+1].via=terminal3;///terminal 3 blockage clear
			layout_matrix[temp_h+6][temp_v+4].via=0;///terminal 4 down blockage clear
			layout_matrix[temp_h+5][temp_v+4].via=terminal4;///terminal 4 blockage clear
            block_cell_terminals(temp_h,temp_v,c_numb);
		 }
	   }
  }
//  cout<<"cell terminal "<<cell_terminal_vector.at(41).t[0].x<<endl;
}



///*******reading placement from the file*****///
void initializing_placement_matrix()
{
int p;
ifstream placement(placementinput[1]);
if(placement.is_open())
{

 for(int j=0;j<RR;++j)
    {
        for(int i=0;i<CC;++i)
        {
            placement>>p;
            if(p!=-1){
			p_matrix.at(j).at(i).cell_number=p-1;
            } else 

            p_matrix.at(j).at(i).p_grid.x=j;

            p_matrix.at(j).at(i).p_grid.y=i;
            ++p;
        }
    }


} else cout<<"Failed to open the file "<<endl;
placement.close();


}





///*******ordering the nets using number of terminals inside the bounds*************///
bool sort_using_priority(netinfo j,netinfo i)
{
	return j.priority<i.priority;
}

void sort_using_net_priority(vector <netinfo> temp_vector)
{
	int x1,y1,x2,y2,x,y;

    for(int j=0;j<temp_vector.size();++j)
	{
		 x1=cell_terminal_vector.at(temp_vector.at(j).s_cell).t[temp_vector.at(j).s_terminal].x;
		 y1=cell_terminal_vector.at(temp_vector.at(j).s_cell).t[temp_vector.at(j).s_terminal].y;
		 x2=cell_terminal_vector.at(temp_vector.at(j).t_cell).t[temp_vector.at(j).t_terminal].x;
		 y2=cell_terminal_vector.at(temp_vector.at(j).t_cell).t[temp_vector.at(j).t_terminal].y;

	    for(int i=0;i<temp_vector.size();++i)
		{
			x=cell_terminal_vector.at(temp_vector.at(i).s_cell).t[temp_vector.at(i).s_terminal].x;
			y=cell_terminal_vector.at(temp_vector.at(i).s_cell).t[temp_vector.at(i).s_terminal].y;

			if((x<x1 && x>x2) || (x>x1 && x<x2))
			{
				if((y<y1 && y>y2) || (y>y1 && y<y2))
				{
			      ++temp_vector.at(j).priority;
			    }
			}
		}
	}

   sort(temp_vector.begin(),temp_vector.end(),sort_using_priority);
}




///*******ordering the nets using net length*************///

bool sort_using_net_length(const netinfo &j,const netinfo &i)
{

int x1=cell_terminal_vector.at(j.s_cell).t[j.s_terminal].x;
int y1=cell_terminal_vector.at(j.s_cell).t[j.s_terminal].y;
int x2=cell_terminal_vector.at(j.t_cell).t[j.t_terminal].x;
int y2=cell_terminal_vector.at(j.t_cell).t[j.t_terminal].y;

int x3=cell_terminal_vector.at(i.s_cell).t[i.s_terminal].x;
int y3=cell_terminal_vector.at(i.s_cell).t[i.s_terminal].y;
int x4=cell_terminal_vector.at(i.t_cell).t[i.t_terminal].x;
int y4=cell_terminal_vector.at(i.t_cell).t[i.t_terminal].y;
manhattendistance+=abs(x1-x2)+abs(y1-y2)<abs(x3-x4)+abs(y3-y4);
return abs(x1-x2)+abs(y1-y2)<abs(x3-x4)+abs(y3-y4);

}



////*******to calculate distance from the center of the layout******///
bool distance(int x1,int y1,int x2,int y2)
{
int x3=layout_RR/2;
int y3=layout_CC/2;

int distance1=(x3-x1)^2+(y3-y1)^2;
int distance2=(x3-x2)^2+(y3-y2)^2;
	if(distance1>distance2)
	{
		return true;
	} else return false;
}

///*****************routing function*****************///
void routing(int choice)
{
int true_f,sucess_count=0;
int hs,vs,ht,vt,netnumb,sc,st,tc,tt;

vector <netinfo> temp_vector;

	if(choice==0)
	{
		temp_vector=net_vector;
	}
	else if(choice==1)
	{
		temp_vector=unrouted_nets;

	}

//random_shuffle(temp_vector.begin(),temp_vector.end());
sort(temp_vector.begin(),temp_vector.end(),sort_using_net_length);
//sort_using_net_priority(temp_vector);
int size=temp_vector.size();
for(int j=0;j<size;++j)
{
netnumb=temp_vector.at(j).net_number;

sc=temp_vector.at(j).s_cell;
st=temp_vector.at(j).s_terminal;
tc=temp_vector.at(j).t_cell;
tt=temp_vector.at(j).t_terminal;
hs=cell_terminal_vector.at(sc).t[st].x;
vs=cell_terminal_vector.at(sc).t[st].y;
ht=cell_terminal_vector.at(tc).t[tt].x;
vt=cell_terminal_vector.at(tc).t[tt].y;
clear_cell_terminals(st,hs,vs);
clear_cell_terminals(tt,ht,vt);

/*if(!distance(hs,vs,ht,vt))
{
 swap(hs,ht);
 swap(vs,vt);
}*/
      
if(choice==0)true_f= findpath(netnumb,hs,vs,ht,vt);
       if(choice==1)true_f= findpath(netnumb,ht,vt,hs,vs);

        switch(true_f)
		{
			case success:// cout<<" FINDING THE PATH SUCCESSFUL FOR NET  "<<temp_vector.at(j).net_number<<" sc: "<<temp_vector.at(j).s_cell+1<<" dc: "<<temp_vector.at(j).t_cell+1<<endl;
                            ++sucess_count;
			                break;

			case f_findpath: cout<<"---- Failed in finding the path for net ---- "<<temp_vector.at(j).net_number<<" sc: "<<temp_vector.at(j).s_cell+1<<" dc: "<<temp_vector.at(j).t_cell+1<<endl;

                              if(choice==0){unrouted_nets.push_back(temp_vector.at(j));}
							  else if(choice==1){unrouted_nets_2.push_back(temp_vector.at(j));}
							         if(choice==0)clearlee(netnumb,hs,vs,ht,vt);
                                     if(choice==1)clearlee(netnumb,ht,vt,hs,vs);
                              break;

			case f_backtarce: cout<<"---- Failed in back tracing for net ---- "<<temp_vector.at(j).net_number<<" sc: "<<temp_vector.at(j).s_cell+1<<" dc: "<<temp_vector.at(j).t_cell+1<<endl;
                              if(choice==0){unrouted_nets.push_back(temp_vector.at(j));}
							  else if(choice==1){unrouted_nets_2.push_back(temp_vector.at(j));}
							         if(choice==0)backtrace_clear(netnumb,hs,vs,ht,vt);
                                     if(choice==1)backtrace_clear(netnumb,ht,vt,hs,vs);
							   if(choice==0)clearlee(netnumb,hs,vs,ht,vt);
							   if(choice==1)clearlee(netnumb,ht,vt,hs,vs);
							  break;

			case f_clearance: cout<<"---- Failed in clearance for temp_vector ---- "<<temp_vector.at(j).net_number<<" sc: "<<temp_vector.at(j).s_cell+1<<" dc: "<<temp_vector.at(j).t_cell+1<<endl;;break;
			default: cout<<"problem with the return "<<endl;
		}
}
if(choice==0)
cout<<"------ ' number of failed nets '-------" <<unrouted_nets.size()<<endl;
if(choice==1)
cout<<"------ ' number of failed nets '-------" <<unrouted_nets_2.size()<<endl;

}


///*********checking terminal blocked or not*************///
bool ctc(int h,int v,int sc,int st,int tc,int tt)
{
	if(layout_matrix.at(h).at(v).cellnumber==-1 /*&& layout_matrix.at(h).at(v).via==0*/)
	 {
	    return true;
	 }
	 else if ((layout_matrix.at(h).at(v).cellnumber==sc /*&& layout_matrix.at(h).at(v).via==st+5*/) || (layout_matrix.at(h).at(v).cellnumber==tc /*&& layout_matrix.at(h).at(v).via==tt+5*/))
     {
    	return true;
     } else return false;
}

///************clearing the terminal blockages***************///
void clear_cell_terminals(int tnumb,int h,int v)
{
	int length=5;
	int j=2;
	switch(tnumb+1)
	{
		case 1:   ///terminal 1
		for(int i=j;i<length;++i)
		{
			layout_matrix.at(h-i).at(v-1).via1=0;
            layout_matrix.at(h-i).at(v+1).via1=0;
		}
		break;
		///terminal 2
		case 2:
		for(int i=j;i<length;++i)
		{
		layout_matrix.at(h-i).at(v+1).via1=0;
		layout_matrix.at(h-i).at(v-1).via1=0;

	    }
		break;

	    case 3:///terminal 3
		for(int i=j;i<length;++i)
		{

			layout_matrix.at(h+i).at(v+1).via1=0;

			layout_matrix.at(h+i).at(v-1).via1=0;
		}
		break;

		case 4:///terminal 4
		for(int i=j;i<length;++i)
		{
	        layout_matrix.at(h+i).at(v+1).via1=0;

			layout_matrix.at(h+i).at(v-1).via1=0;
		}

		break;
	    default:cout<<" You forgot something!!!!"<<endl;
	}
}


///************Blocking the cell terminals***************///
void block_cell_terminals(int h,int v,int cellnumber)
{
int length=5;
int j=2;
	for(int i=j;i<length;++i)
	{
		///terminal 1

        layout_matrix.at(h-i).at(v+1-1).via1=cell_blockage;

		layout_matrix.at(h-i).at(v+1+1).via1=cell_blockage;

        ///terminal 2

		layout_matrix.at(h-i).at(v+4+1).via1=cell_blockage;
		layout_matrix.at(h-i).at(v+4-1).via1=cell_blockage;

		///terminal 3
		layout_matrix.at(h+5+i).at(v+1+1).via1=cell_blockage;
		layout_matrix.at(h+5+i).at(v+1-1).via1=cell_blockage;
		///terminal 4

		layout_matrix.at(h+5+i).at(v+4+1).via1=cell_blockage;
		layout_matrix.at(h+5+i).at(v+4-1).via1=cell_blockage;

	}

//    for(int i=4;i<7;++i)
//	{
//		///terminal 1
//		 layout_matrix.at(h-i).at(v+1-1).via1=cell_blockage;
//         layout_matrix.at(h-i).at(v+1+1).via1=cell_blockage;
//
//		///terminal 4
//		layout_matrix.at(h+5+i).at(v+4+1).via1=cell_blockage;
//		layout_matrix.at(h+5+i).at(v+4-1).via1=cell_blockage;
//
//	}

}




///**********propagating lee number ************///
int findpath(int netnumb,int hs,int vs,int ht,int vt)
{
////cout<<"Trying to find the Path for net_vector "<<netnumb<<endl;
//////cout<<"Manhatten distance: "<<(ht-hs)+(vt-vs)<<endl;
int h,v,i;

layout_matrix.at(hs).at(vs).LeeNumb=0;
propagatelist.push_back(layout_matrix.at(hs).at(vs));
h=propagatelist[0].xcoordinate;
v=propagatelist[0].ycoordinate;
while(!propagatelist.empty())
{
   i=0;
   if(propagatelist[0].xcoordinate==ht && propagatelist[0].ycoordinate==vt)
    {
	////cout<<"Path found :)"<<endl;
	////cout<<"distance from the source "<<propagatelist[0].LeeNumb<<endl;
    ////cout<<"left: "<<"x: "<<propagatelist[0].xcoordinate<<" y: "<<propagatelist[0].ycoordinate<<" Lee: "<<propagatelist[0].LeeNumb<<endl;
	propagatelist.clear();
	return backtrace(netnumb,hs,vs,ht,vt);
	}
  else
   {

		h=propagatelist[0].xcoordinate;
		v=propagatelist[0].ycoordinate;

		//left
		if(h<layout_RR && h>=0 && v-1<layout_CC && v-1>=0)
		{
			if(((layout_matrix.at(h).at(v-1).via!=cell) && (layout_matrix.at(h).at(v-1).via!=cell_blockage) && (layout_matrix.at(h).at(v-1).via!=via_blockage)) && (layout_matrix.at(h).at(v-1).LeeNumb==-2))
			{
		//	    //cout<<"left: "<<"x: "<<propagatelist[0].xcoordinate<<" y: "<<propagatelist[0].ycoordinate<<" Lee: "<<propagatelist[0].LeeNumb<<endl;
			   if(layout_matrix.at(h).at(v-1).via1!=cell_blockage){
			   layout_matrix.at(h).at(v-1).LeeNumb=layout_matrix.at(h).at(v).LeeNumb+1;
			   propagatelist.push_back(layout_matrix.at(h).at(v-1));}
			}
		++i;
		}

		//right
		if(h<layout_RR && h>=0 && v+1<layout_CC && v+1>=0)
		{
			if(((layout_matrix.at(h).at(v+1).via!=cell) && (layout_matrix.at(h).at(v+1).via!=cell_blockage) && (layout_matrix.at(h).at(v+1).via!=via_blockage))  && (layout_matrix.at(h).at(v+1).LeeNumb==-2))
			{
			//	//cout<<"left: "<<"x: "<<propagatelist[0].xcoordinate<<" y: "<<propagatelist[0].ycoordinate<<" Lee: "<<propagatelist[0].LeeNumb<<endl;
				if(layout_matrix.at(h).at(v+1).via1!=cell_blockage) {
				layout_matrix.at(h).at(v+1).LeeNumb=layout_matrix.at(h).at(v).LeeNumb+1;
				propagatelist.push_back(layout_matrix.at(h).at(v+1));}
			}
		++i;
		}


		//up
		if(h-1<layout_RR && h-1>=0 && v<layout_CC && v>=0)
		{
			if(((layout_matrix.at(h-1).at(v).via!=cell) && (layout_matrix.at(h-1).at(v).via!=cell_blockage) && (layout_matrix.at(h-1).at(v).via!=via_blockage))  && (layout_matrix.at(h-1).at(v).LeeNumb==-2))
			{
			//    //cout<<"left: "<<"x: "<<propagatelist[0].xcoordinate<<" y: "<<propagatelist[0].ycoordinate<<" Lee: "<<propagatelist[0].LeeNumb<<endl;
			   if(layout_matrix.at(h-1).at(v).via1!=cell_blockage){
			   layout_matrix.at(h-1).at(v).LeeNumb=layout_matrix.at(h).at(v).LeeNumb+1;
			   propagatelist.push_back(layout_matrix.at(h-1).at(v));}
			}
		++i;
		}

		  //down
		  if(h+1<layout_RR && h+1>=0 && v<layout_CC && v>=0)
		  {
			// //cout<<"left: "<<"x: "<<propagatelist[0].xcoordinate<<" y: "<<propagatelist[0].ycoordinate<<" Lee: "<<propagatelist[0].LeeNumb<<endl;
			if(((layout_matrix.at(h+1).at(v).via!=cell) && (layout_matrix.at(h+1).at(v).via!=cell_blockage) && (layout_matrix.at(h+1).at(v).via!=via_blockage))  && (layout_matrix.at(h+1).at(v).LeeNumb==-2))
			{
				 if(layout_matrix.at(h+1).at(v).via1!=cell_blockage) {
				layout_matrix.at(h+1).at(v).LeeNumb=layout_matrix.at(h).at(v).LeeNumb+1;
				propagatelist.push_back(layout_matrix.at(h+1).at(v));}
			}
		  ++i;
		  }

   propagatelist.erase(propagatelist.begin());
   }///else block ends

	if(i==0)
	{
		propagatelist.clear();
	}
}///while(propagation list empty block ends
return f_findpath;
}

///************Creating the via at the bend*********************////
void via_side_blockage(char choice,int h,int v,int netnumb)
{
	if(choice =='h')
	{
		if(cb(h,v)){layout_matrix.at(h).at(v).via=via_at_bend;layout_matrix.at(h).at(v).net_ref[hor]=netnumb;}
		if(cb(h+1,v)){layout_matrix.at(h+1).at(v).via=via_blockage;layout_matrix.at(h+1).at(v).net_ref[hor]=netnumb;}
		if(cb(h-1,v)){layout_matrix.at(h-1).at(v).via=via_blockage;layout_matrix.at(h-1).at(v).net_ref[hor]=netnumb;}
		if(cb(h,v+1)){layout_matrix.at(h).at(v+1).via=via_blockage;layout_matrix.at(h).at(v+1).net_ref[hor]=netnumb;}
		if(cb(h,v-1)){layout_matrix.at(h).at(v-1).via=via_blockage;layout_matrix.at(h).at(v-1).net_ref[hor]=netnumb;}
		if(cb(h+1,v-1)){layout_matrix.at(h+1).at(v-1).via=via_blockage;layout_matrix.at(h+1).at(v-1).net_ref[hor]=netnumb;}
		if(cb(h+1,v+1)){layout_matrix.at(h+1).at(v+1).via=via_blockage;layout_matrix.at(h+1).at(v+1).net_ref[hor]=netnumb;}
		if(cb(h-1,v-1)){layout_matrix.at(h-1).at(v-1).via=via_blockage;layout_matrix.at(h-1).at(v-1).net_ref[hor]=netnumb;}
		if(cb(h-1,v+1)){layout_matrix.at(h-1).at(v+1).via=via_blockage;layout_matrix.at(h-1).at(v+1).net_ref[hor]=netnumb;}
	}
	else if(choice=='v')
	{
		if(cb(h,v)){layout_matrix.at(h).at(v).via=via_at_bend;layout_matrix.at(h).at(v).net_ref[ver]=netnumb;}
		if(cb(h+1,v)){layout_matrix.at(h+1).at(v).via=via_blockage;layout_matrix.at(h+1).at(v).net_ref[ver]=netnumb;}
		if(cb(h-1,v)){layout_matrix.at(h-1).at(v).via=via_blockage;layout_matrix.at(h-1).at(v).net_ref[ver]=netnumb;}
		if(cb(h,v+1)){layout_matrix.at(h).at(v+1).via=via_blockage;layout_matrix.at(h).at(v+1).net_ref[ver]=netnumb;}
		if(cb(h,v-1)){layout_matrix.at(h).at(v-1).via=via_blockage;layout_matrix.at(h).at(v-1).net_ref[ver]=netnumb;}
		if(cb(h+1,v-1)){layout_matrix.at(h+1).at(v-1).via=via_blockage;layout_matrix.at(h+1).at(v-1).net_ref[ver]=netnumb;}
		if(cb(h+1,v+1)){layout_matrix.at(h+1).at(v+1).via=via_blockage;layout_matrix.at(h+1).at(v+1).net_ref[ver]=netnumb;}
		if(cb(h-1,v-1)){layout_matrix.at(h-1).at(v-1).via=via_blockage;layout_matrix.at(h-1).at(v-1).net_ref[ver]=netnumb;}
		if(cb(h-1,v+1)){layout_matrix.at(h-1).at(v+1).via=via_blockage;layout_matrix.at(h-1).at(v+1).net_ref[ver]=netnumb;}
	}
}



///***********Back tracing the path***************///
int backtrace(int netnumb,int hs,int vs,int ht,int vt)
{
int i;
////cout<<"Trying to Back trace for net_vector "<<netnumb<<endl;
	int h,v;//to store target coordinates
	propagatelist.push_back(layout_matrix[ht][vt]);
        ////////////////////////////// newly added

//        layout_matrix[ht][vt].netnumb=netnumb;
//        //////////////////////////////
		h=propagatelist[0].xcoordinate;
	    v=propagatelist[0].ycoordinate;

  while( !propagatelist.empty())
   {
     i=0;
     if((propagatelist[0].xcoordinate==hs) && (propagatelist[0].ycoordinate==vs) )
       {
//	    ////cout<<"final step "<<"x: "<<propagatelist[0].xcoordinate<<" y: "<<propagatelist[0].ycoordinate<<" Lee: "<<propagatelist[0].LeeNumb<<endl;
            ////cout<<"Back trace Path found :')"<<endl;

		layout_matrix.at(h).at(v).netnumb[ver]=netnumb;
        propagatelist.clear();
		return clearlee(netnumb,hs,vs,ht,vt);

	   }
	 else
	 {
       //left
       while((h<layout_RR && h>=0 && v-1<layout_CC && v-1>=0) && (layout_matrix.at(h).at(v-1).b[hor]==0)&&(layout_matrix.at(h).at(v).LeeNumb-layout_matrix.at(h).at(v-1).LeeNumb==1))
		{

			propagatelist.push_back(layout_matrix.at(h).at(v-1));
		    propagatelist.erase(propagatelist.begin());
//			////cout<<"left: "<<"x: "<<propagatelist[0].xcoordinate<<" y: "<<propagatelist[0].ycoordinate<<" Lee: "<<propagatelist[0].LeeNumb<<endl;
            h=propagatelist[0].xcoordinate;
	        v=propagatelist[0].ycoordinate;
	        layout_matrix.at(h).at(v).netnumb[hor]=netnumb;
            layout_matrix.at(h).at(v).b[hor]=1;
			if(h+1<layout_RR )
			{
			  layout_matrix.at(h+1).at(v).b[hor]=1;
			  layout_matrix.at(h+1).at(v).net_ref[hor]=netnumb;
			}
			if( h-1>=0)
			{
			   layout_matrix.at(h-1).at(v).b[hor]=1;
			   layout_matrix.at(h-1).at(v).net_ref[hor]=netnumb;
			}
        ++i;
		}

	if(!((h==hs && v==vs) || (h==ht && v==vt)))
		{
		    if(layout_matrix.at(h).at(v).b[ver]==0){via_side_blockage('h',h,v,netnumb);}


		}

      //right
		while((h<layout_RR && h>=0 && v+1<layout_CC && v+1>=0) && (layout_matrix.at(h).at(v+1).b[hor]==0) && (layout_matrix.at(h).at(v).LeeNumb-layout_matrix.at(h).at(v+1).LeeNumb==1) )
		{
            propagatelist.push_back(layout_matrix.at(h).at(v+1));
          	propagatelist.erase(propagatelist.begin());

		}


		//up
		while((h-1<layout_RR && h-1>=0 && v<layout_CC && v>=0) && (layout_matrix.at(h-1).at(v).b[ver]==0) && (layout_matrix.at(h).at(v).LeeNumb-layout_matrix.at(h-1).at(v).LeeNumb==1) )
		{
			propagatelist.push_back(layout_matrix.at(h-1).at(v));
			propagatelist.erase(propagatelist.begin());

//			////cout<<"up: "<<"x: "<<propagatelist[0].xcoordinate<<" y: "<<propagatelist[0].ycoordinate<<" Lee: "<<propagatelist[0].LeeNumb<<endl;
			h=propagatelist[0].xcoordinate;
	        v=propagatelist[0].ycoordinate;
		    layout_matrix.at(h).at(v).netnumb[ver]=netnumb;
			layout_matrix.at(h).at(v).b[ver]=1;
		    if(v-1>=0)
			{
				  layout_matrix.at(h).at(v-1).b[ver]=1;
				  layout_matrix.at(h).at(v-1).net_ref[ver]=netnumb;
			}

			if(v+1<layout_CC)
			{
				layout_matrix.at(h).at(v+1).b[ver]=1;
				layout_matrix.at(h).at(v+1).net_ref[ver]=netnumb;
			}
		++i;
		}

	if(!((h==hs && v==vs) || (h==ht && v==vt)))
		{
		     if(layout_matrix.at(h).at(v).b[hor]==0){via_side_blockage('v',h,v,netnumb);}


		}


      //down
       while((h+1<layout_RR && h+1>=0 && v<layout_CC && v>=0)&& (layout_matrix.at(h+1).at(v).b[ver]==0) && (layout_matrix.at(h).at(v).LeeNumb-layout_matrix.at(h+1).at(v).LeeNumb==1))
		{
			propagatelist.push_back(layout_matrix.at(h+1).at(v));
			 propagatelist.erase(propagatelist.begin());
//			////cout<<"down: "<<"x: "<<propagatelist[0].xcoordinate<<" y: "<<propagatelist[0].ycoordinate<<" Lee: "<<propagatelist[0].LeeNumb<<endl;

			h=propagatelist[0].xcoordinate;
	        v=propagatelist[0].ycoordinate;
		    layout_matrix.at(h).at(v).netnumb[ver]=netnumb;
			layout_matrix.at(h).at(v).b[ver]=1;
		    if(v-1>=0)
			{
				  layout_matrix.at(h).at(v-1).b[ver]=1;
				  layout_matrix.at(h).at(v-1).net_ref[ver]=netnumb;
			}
		   if(v+1<layout_CC)
			{
				layout_matrix.at(h).at(v+1).b[ver]=1;
				layout_matrix.at(h).at(v+1).net_ref[ver]=netnumb;
			}
		++i;
		}


		if(!((h==hs && v==vs) || (h==ht && v==vt)))
		{
		     if(layout_matrix.at(h).at(v).b[hor]==0){via_side_blockage('v',h,v,netnumb);}



		}


		if(i==0)
		{
			propagatelist.clear();
		}

    }//else block ends
  }//while block ends
return f_backtarce;
}



///********clearing the lee number ********///
int clearlee(int netnumb,int hs,int vs,int ht,int vt)
{

////cout<<"Trying to clear for net_vector "<<netnumb<<endl;
//layout_matrix.at(hs).at(vs).blocked=1;

int h,v;
propagatelist.push_back(layout_matrix.at(hs).at(vs));

while(!propagatelist.empty())
{
  if((propagatelist[0].xcoordinate==ht) && (propagatelist[0].ycoordinate==vt))
   {
	//////cout<<"target in clearance phase "<<propagatelist[0].xcoordinate<<' '<<propagatelist[0].ycoordinate<<endl;
	////cout<<"Clearing process is complete :'')"<<endl;

	propagatelist.clear();
	return success;
   }
  else
   {
   //	////cout<<"Trying to Clear the Lee numbers "<<endl;
   h=propagatelist[0].xcoordinate;
   v=propagatelist[0].ycoordinate;

//left
if(h<layout_RR && h>=0 && v-1<layout_CC && v-1>=0)
{
   if(layout_matrix.at(h).at(v-1).LeeNumb!=-2)
	{
        propagatelist.push_back(layout_matrix.at(h).at(v-1));
		// ////cout<<"down "<<"x: "<<propagatelist[0].xcoordinate<<" y: "<<propagatelist[0].ycoordinate<<endl;
        layout_matrix.at(h).at(v-1).LeeNumb=-2;
	}
}
//up ends
///right
if(h<layout_RR && h>=0 && v+1<layout_CC && v+1>=0)
{
   if(layout_matrix.at(h).at(v+1).LeeNumb!=-2 )
	{
        propagatelist.push_back(layout_matrix.at(h).at(v+1));
		//////cout<<"up "<<"x: "<<propagatelist[0].xcoordinate<<" y: "<<propagatelist[0].ycoordinate<<endl;
        layout_matrix.at(h).at(v+1).LeeNumb=-2;
	}
}
//down ends
///up
if(h-1<layout_RR && h-1>=0 && v<layout_CC && v>=0)
{
	if(layout_matrix.at(h-1).at(v).LeeNumb!=-2 )
	{

		propagatelist.push_back(layout_matrix.at(h-1).at(v));

//////cout<<"left "<<"x: "<<propagatelist[0].xcoordinate<<" y: "<<propagatelist[0].ycoordinate<<endl;
		layout_matrix.at(h-1).at(v).LeeNumb=-2;
	}
}
//left ends
///down
if(h+1<layout_RR && h+1>=0 && v<layout_CC && v>=0)
{
	if(layout_matrix.at(h+1).at(v).LeeNumb!=-2 )
	{
		propagatelist.push_back(layout_matrix.at(h+1).at(v));
//////cout<<"right "<<"x: "<<propagatelist[0].xcoordinate<<" y: "<<propagatelist[0].ycoordinate<<endl;
        layout_matrix.at(h+1).at(v).LeeNumb=-2;
	}
}
//right ends


propagatelist.erase(propagatelist.begin());
   }//else block ends

}//while ends
return f_clearance;
}

///************Clearing the via at the bend*********************////
void via_side_blockage_clear(char choice,int h,int v,int netnumb)
{
	if(choice =='h')
	{
		if(cb(h,v) && layout_matrix.at(h).at(v).net_ref[hor]==netnumb ){layout_matrix.at(h).at(v).via=0;}
		if(cb(h+1,v) && layout_matrix.at(h+1).at(v).net_ref[hor]==netnumb){layout_matrix.at(h+1).at(v).via=0;}
		if(cb(h-1,v)&& layout_matrix.at(h-1).at(v).net_ref[hor]==netnumb){layout_matrix.at(h-1).at(v).via=0;}
		if(cb(h,v+1)&& layout_matrix.at(h).at(v+1).net_ref[hor]==netnumb){layout_matrix.at(h).at(v+1).via=0;}
		if(cb(h,v-1)&& layout_matrix.at(h).at(v-1).net_ref[hor]==netnumb){layout_matrix.at(h).at(v-1).via=0;}
		if(cb(h+1,v-1)&& layout_matrix.at(h+1).at(v-1).net_ref[hor]==netnumb){layout_matrix.at(h+1).at(v-1).via=0;}
		if(cb(h+1,v+1)&& layout_matrix.at(h+1).at(v+1).net_ref[hor]==netnumb){layout_matrix.at(h+1).at(v+1).via=0;}
		if(cb(h-1,v-1)&& layout_matrix.at(h-1).at(v-1).net_ref[hor]==netnumb){layout_matrix.at(h-1).at(v-1).via=0;}
		if(cb(h-1,v+1)&& layout_matrix.at(h-1).at(v+1).net_ref[hor]==netnumb){layout_matrix.at(h-1).at(v+1).via=0;}
	}
	else if(choice=='v')
	{
		if(cb(h,v) && layout_matrix.at(h).at(v).net_ref[ver]==netnumb ){layout_matrix.at(h).at(v).via=0;}
		if(cb(h+1,v) && layout_matrix.at(h+1).at(v).net_ref[ver]==netnumb){layout_matrix.at(h+1).at(v).via=0;}
		if(cb(h-1,v)&& layout_matrix.at(h-1).at(v).net_ref[ver]==netnumb){layout_matrix.at(h-1).at(v).via=0;}
		if(cb(h,v+1)&& layout_matrix.at(h).at(v+1).net_ref[ver]==netnumb){layout_matrix.at(h).at(v+1).via=0;}
		if(cb(h,v-1)&& layout_matrix.at(h).at(v-1).net_ref[ver]==netnumb){layout_matrix.at(h).at(v-1).via=0;}
		if(cb(h+1,v-1)&& layout_matrix.at(h+1).at(v-1).net_ref[ver]==netnumb){layout_matrix.at(h+1).at(v-1).via=0;}
		if(cb(h+1,v+1)&& layout_matrix.at(h+1).at(v+1).net_ref[ver]==netnumb){layout_matrix.at(h+1).at(v+1).via=0;}
		if(cb(h-1,v-1)&& layout_matrix.at(h-1).at(v-1).net_ref[ver]==netnumb){layout_matrix.at(h-1).at(v-1).via=0;}
		if(cb(h-1,v+1)&& layout_matrix.at(h-1).at(v+1).net_ref[ver]==netnumb){layout_matrix.at(h-1).at(v+1).via=0;}

	}
}


///*************clearing the unrouted nets*******************///
int backtrace_clear(int netnumb,int hs,int vs,int ht,int vt)
{
int i;
////cout<<"Trying to Back trace for net_vector "<<netnumb<<endl;
	int h,v;//to store target coordinates
	propagatelist.push_back(layout_matrix[ht][vt]);
        ////////////////////////////// newly added

//        layout_matrix[ht][vt].netnumb=netnumb;
//        //////////////////////////////
		h=propagatelist[0].xcoordinate;
	    v=propagatelist[0].ycoordinate;

  while( !propagatelist.empty())
   {
     i=0;
     if((propagatelist[0].xcoordinate==hs) && (propagatelist[0].ycoordinate==vs) )
       {
//	    ////cout<<"final step "<<"x: "<<propagatelist[0].xcoordinate<<" y: "<<propagatelist[0].ycoordinate<<" Lee: "<<propagatelist[0].LeeNumb<<endl;
            ////cout<<"Back trace Path found :')"<<endl;

		layout_matrix.at(h).at(v).netnumb[ver]=-1;
        propagatelist.clear();
		return clearlee(netnumb,hs,vs,ht,vt);

	   }
	 else
	 {
       //left

       while(cb(h,v-1) && (layout_matrix.at(h).at(v-1).netnumb[hor]==netnumb)&&(layout_matrix.at(h).at(v-1).b[hor]==1)&&(layout_matrix.at(h).at(v).LeeNumb-layout_matrix.at(h).at(v-1).LeeNumb==1))
		{

			propagatelist.push_back(layout_matrix.at(h).at(v-1));
		    propagatelist.erase(propagatelist.begin());
//			cout<<"left: "<<"x: "<<propagatelist[0].xcoordinate<<" y: "<<propagatelist[0].ycoordinate<<" Lee: "<<propagatelist[0].LeeNumb<<endl;
            h=propagatelist[0].xcoordinate;
	        v=propagatelist[0].ycoordinate;
	        layout_matrix.at(h).at(v).netnumb[hor]=-1;
            layout_matrix.at(h).at(v).b[hor]=0;
			if(h+1<layout_RR && layout_matrix.at(h+1).at(v).net_ref[hor]==netnumb)
			{
			  layout_matrix.at(h+1).at(v).b[hor]=0;
			}
			if( h-1>=0 && layout_matrix.at(h-1).at(v).net_ref[hor]==netnumb)
			{
			   layout_matrix.at(h-1).at(v).b[hor]=0;
			}
        ++i;
		}

	if(!((h==hs && v==vs) || (h==ht && v==vt)))
		{
		    via_side_blockage_clear('h',h,v,netnumb);

		}

      //right
		while(cb(h,v+1)&& (layout_matrix.at(h).at(v+1).netnumb[hor]==netnumb) && (layout_matrix.at(h).at(v+1).b[hor]==1) && (layout_matrix.at(h).at(v).LeeNumb-layout_matrix.at(h).at(v+1).LeeNumb==1) )
		{
            propagatelist.push_back(layout_matrix.at(h).at(v+1));
          	propagatelist.erase(propagatelist.begin());

//          	cout<<"right: "<<"x: "<<propagatelist[0].xcoordinate<<" y: "<<propagatelist[0].ycoordinate<<" Lee: "<<propagatelist[0].LeeNumb<<endl;

			h=propagatelist[0].xcoordinate;
	        v=propagatelist[0].ycoordinate;
	        layout_matrix.at(h).at(v).netnumb[hor]=-1;
            layout_matrix.at(h).at(v).b[hor]=0;

			if(h+1<layout_RR && layout_matrix.at(h+1).at(v).net_ref[hor]==netnumb )
			{
			  layout_matrix.at(h+1).at(v).b[hor]=0;
			}
			if( h-1>=0 && layout_matrix.at(h-1).at(v).net_ref[hor]==netnumb)
			{
			   layout_matrix.at(h-1).at(v).b[hor]=0;
			}
		++i;
		}


	if(!((h==hs && v==vs) || (h==ht && v==vt)))
		{
		    via_side_blockage_clear('h',h,v,netnumb);

		}


		//up
		while(cb(h-1,v)&& (layout_matrix.at(h-1).at(v).netnumb[ver]==netnumb) && (layout_matrix.at(h-1).at(v).b[ver]==1) && (layout_matrix.at(h).at(v).LeeNumb-layout_matrix.at(h-1).at(v).LeeNumb==1) )
		{
			propagatelist.push_back(layout_matrix.at(h-1).at(v));
			propagatelist.erase(propagatelist.begin());

//			cout<<"up: "<<"x: "<<propagatelist[0].xcoordinate<<" y: "<<propagatelist[0].ycoordinate<<" Lee: "<<propagatelist[0].LeeNumb<<endl;
			h=propagatelist[0].xcoordinate;
	        v=propagatelist[0].ycoordinate;
		    layout_matrix.at(h).at(v).netnumb[ver]=-1;
			layout_matrix.at(h).at(v).b[ver]=0;
		    if(v-1>=0 && layout_matrix.at(h).at(v-1).net_ref[ver]==netnumb)
			{
				  layout_matrix.at(h).at(v-1).b[ver]=0;
			}

			if(v+1<layout_CC && layout_matrix.at(h).at(v+1).net_ref[ver]==netnumb)
			{
				layout_matrix.at(h).at(v+1).b[ver]=0;
			}
		++i;
		}

	if(!((h==hs && v==vs) || (h==ht && v==vt)))
		{
		    via_side_blockage_clear('v',h,v,netnumb);

		}


      //down
       while(cb(h+1,v)&& (layout_matrix.at(h+1).at(v).netnumb[ver]==netnumb) && (layout_matrix.at(h+1).at(v).b[ver]==1) && (layout_matrix.at(h).at(v).LeeNumb-layout_matrix.at(h+1).at(v).LeeNumb==1))
		{
			propagatelist.push_back(layout_matrix.at(h+1).at(v));
			 propagatelist.erase(propagatelist.begin());
//			cout<<"down: "<<"x: "<<propagatelist[0].xcoordinate<<" y: "<<propagatelist[0].ycoordinate<<" Lee: "<<propagatelist[0].LeeNumb<<endl;

			h=propagatelist[0].xcoordinate;
	        v=propagatelist[0].ycoordinate;
		    layout_matrix.at(h).at(v).netnumb[ver]=-1;
			layout_matrix.at(h).at(v).b[ver]=0;
		    if(v-1>=0  && layout_matrix.at(h).at(v-1).net_ref[ver]==netnumb)
			{
				  layout_matrix.at(h).at(v-1).b[ver]=0;
			}
		   if(v+1<layout_CC  && layout_matrix.at(h).at(v+1).net_ref[ver]==netnumb )
			{
				layout_matrix.at(h).at(v+1).b[ver]=0;
			}
		++i;
		}


		if(!((h==hs && v==vs) || (h==ht && v==vt)))
		{
		    via_side_blockage_clear('v',h,v,netnumb);

		}


		if(i==0)
		{
			propagatelist.clear();
		}

    }//else block ends
  }//while block ends
return f_backtarce;
}






////**************writing output to the magic file*******************////
void Outputfile()
{
  ofstream OutputFile(outputfilename[1]);

	if(OutputFile.is_open())
	{
       		    OutputFile<<"magic"<< endl;
                OutputFile<<"tech scmos"<< endl;
        for(int i=0;i<layout_RR;++i)
		{
           for(int j=0;j<layout_CC;++j)
			{
				if(layout_matrix.at(i).at(j).netnumb[hor]!=-1 && layout_matrix.at(i).at(j).b[hor] ==1 )
				{
					++finalwirelength;
                    OutputFile<<"<< m1 >>"<<endl;
					OutputFile<<"rect "<<j<<" "<<-(i+1)<<" "<<j+1<<" "<<-i<<" "<<endl;
				}
				if(layout_matrix.at(i).at(j).netnumb[ver]!=-1 && layout_matrix.at(i).at(j).b[ver] == 1 )
				{
					++finalwirelength;
                    OutputFile<<"<< m2 >>"<<endl;
					OutputFile<<"rect "<<j<<" "<<-(i+1)<<" "<<j+1<<" "<<-i<<" "<<endl;
				}
				if(layout_matrix.at(i).at(j).via == via_at_bend)
				{
					++numberofvias;
                    OutputFile<<"<< m2c >>"<<endl;
                    OutputFile<<"rect "<<j<<" "<<-(i+1)<<" "<<j+1<<" "<<-i<<" "<<endl;
				}
				if(layout_matrix.at(i).at(j).via ==cell )
				{
					OutputFile<<"<< pdiffusion >>"<<endl;
					OutputFile<<"rect "<<j<<" "<<-(i+1)<<" "<<j+1<<" "<<-i<<" "<<endl;
				}

				 if(layout_matrix.at(i).at(j).via ==terminal1 && layout_matrix.at(i).at(j-1).via ==cell)
				{
				    OutputFile<<"<< pdiffusion >>"<<endl;
					OutputFile<<"rect "<<j<<" "<<-(i+1)<<" "<<j+1<<" "<<-i<<" "<<endl;
                    OutputFile<<"<< m1 >>"<<endl;
					OutputFile<<"rect "<<j<<" "<<-(i+1)<<" "<<j+1<<" "<<-i<<" "<<endl;
                    OutputFile<<"<< m2 >>"<<endl;
					OutputFile<<"rect "<<j<<" "<<-(i+1)<<" "<<j+1<<" "<<-i<<" "<<endl;
                    OutputFile<<"<< m2c >>"<<endl;
					OutputFile<<"rect "<<j<<" "<<-(i+1)<<" "<<j+1<<" "<<-i<<" "<<endl;

				    
                    OutputFile<<"<< labels >>"<<endl;
					OutputFile<<"rlabel "<<"pdiffusion "<<j<<" "<<-(i+1)<<" "<<j+1<<" "<<-i<<" "<<" 0 "<<"1"<<endl;


				}
				if(layout_matrix.at(i).at(j).via ==terminal2 && layout_matrix.at(i).at(j-1).via ==cell )
				{

					OutputFile<<"<< pdiffusion >>"<<endl;
				OutputFile<<"rect "<<j<<" "<<-(i+1)<<" "<<j+1<<" "<<-i<<" "<<endl;
                OutputFile<<"<< m1 >>"<<endl;
					OutputFile<<"rect "<<j<<" "<<-(i+1)<<" "<<j+1<<" "<<-i<<" "<<endl;
                    OutputFile<<"<< m2 >>"<<endl;
					OutputFile<<"rect "<<j<<" "<<-(i+1)<<" "<<j+1<<" "<<-i<<" "<<endl;
                    OutputFile<<"<< m2c >>"<<endl;
					OutputFile<<"rect "<<j<<" "<<-(i+1)<<" "<<j+1<<" "<<-i<<" "<<endl;


					OutputFile<<"<< labels >>"<<endl;
                    OutputFile<<"rlabel "<<"pdiffusion "<<j<<" "<<-(i+1)<<" "<<j+1<<" "<<-i<<" "<<" 0 "<<"2"<<endl;


				}
				if(layout_matrix.at(i).at(j).via ==terminal3  && layout_matrix.at(i).at(j-1).via ==cell )
				{
					OutputFile<<"<< pdiffusion >>"<<endl;
					OutputFile<<"rect "<<j<<" "<<-(i+1)<<" "<<j+1<<" "<<-i<<" "<<endl;
                    OutputFile<<"<< m1 >>"<<endl;
					OutputFile<<"rect "<<j<<" "<<-(i+1)<<" "<<j+1<<" "<<-i<<" "<<endl;
                    OutputFile<<"<< m2 >>"<<endl;
					OutputFile<<"rect "<<j<<" "<<-(i+1)<<" "<<j+1<<" "<<-i<<" "<<endl;
                    OutputFile<<"<< m2c >>"<<endl;
					OutputFile<<"rect "<<j<<" "<<-(i+1)<<" "<<j+1<<" "<<-i<<" "<<endl;

					OutputFile<<"<< labels >>"<<endl;
				OutputFile<<"rlabel "<<"pdiffusion "<<j<<" "<<-(i+1)<<" "<<j+1<<" "<<-i<<" "<<" 0 "<<"3"<<endl;


				}
				if(layout_matrix.at(i).at(j).via ==terminal4  && layout_matrix.at(i).at(j-1).via ==cell )
				{

					OutputFile<<"<< pdiffusion >>"<<endl;
					OutputFile<<"rect "<<j<<" "<<-(i+1)<<" "<<j+1<<" "<<-i<<" "<<endl;
                    OutputFile<<"<< m1 >>"<<endl;
					OutputFile<<"rect "<<j<<" "<<-(i+1)<<" "<<j+1<<" "<<-i<<" "<<endl;
                    OutputFile<<"<< m2 >>"<<endl;
					OutputFile<<"rect "<<j<<" "<<-(i+1)<<" "<<j+1<<" "<<-i<<" "<<endl;
                    OutputFile<<"<< m2c >>"<<endl;
					OutputFile<<"rect "<<j<<" "<<-(i+1)<<" "<<j+1<<" "<<-i<<" "<<endl;


					OutputFile<<"<< labels >>"<<endl;
					OutputFile<<"rlabel "<<"pdiffusion "<<j<<" "<<-(i+1)<<" "<<j+1<<" "<<-i<<" "<<" 0 "<<"4"<<endl;


				}
				if(layout_matrix.at(i).at(j).cellnumber !=-1 && layout_matrix.at(i).at(j-1).via ==cell && layout_matrix.at(i-1).at(j).via ==cell)
				{
					OutputFile<<"<< labels >>"<<endl;
					OutputFile<<"rlabel "<<"pdiffusion "<<j<<" "<<-(i+1)<<" "<<j+1<<" "<<-i<<" "<<" 0 "<<layout_matrix.at(i).at(j).cellnumber+1<<endl;

				}
            }
        }
      OutputFile<<"<< end >> "<<endl;
   } else cout<<" Error opening the output file "<<endl;
OutputFile.close();
}

bool cb(int h,int v)
{
    if(h<layout_RR && h>=0 && v<layout_CC && v>=0)
    {
        return true;
    } else return false;
}








///*************Initializations and printing**********///

///***********printing the cell for testing*************////
void PrintCell(char select,int h, int v,int ht ,int vt)
{

////cout<<"layout RR "<<layout_RR<<" layout CC "<<layout_CC<<endl;
switch(select)
 {
    case 'h':////cout<<"----Lee number-Net number-h_blockage at index "<<h<<'*'<<v<<"----"<<endl;
			   cout<<" you are in printing "<<endl;
			   for(int i=h;i<ht;++i)
			   {
					for(int j=v;j<vt;++j)
					 {
						cout<<layout_matrix.at(i).at(j).via<<"   ";
						//////cout<<layout_matrix.at(i).at(j).LeeNumb<<"("<<layout_matrix.at(i).at(j).via<<")"<<"   ";
						////cout/*<<layout_matrix.at(i).at(j).LeeNumb*/<<"("<<layout_matrix.at(i).at(j).netnumb<<")"<<layout_matrix.at(i).at(j).via<<"   ";
					 }cout<<endl;
			   }
			   break;

    case 'v':////cout<<"----Net number-v_blockage at index "<<h<<'*'<<v<<"----"<<endl;
			   for(int i=h;i<ht;++i)
			   {
					for(int j=v;j<vt;++j)
					 {
						//cout<<layout_matrix.at(i).at(j).LeeNumb<<"("<<layout_matrix.at(i).at(j).via<<")"<<"   ";
						////cout<<"("<<p_matrix[i][j].netnumb<<")"<<p_matrix[i][j].b[ver]<<"   ";
					 }//cout<<endl;
			   }
			   break;

    case 'x':////cout<<"----Net number-via at index "<<h<<'*'<<v<<"----"<<endl;
               for(int i=h;i<ht;++i)
			   {
					for(int j=v;j<vt;++j)
					 {
					     //cout<<layout_matrix.at(i).at(j).netnumb<<"("<<layout_matrix.at(i).at(j).via<<")"<<"   ";
//						//cout<<layout_matrix.at(i).at(j).netnumb<<"("<<layout_matrix.at(i).at(j).via<<")"<<"   ";
						////cout<<"("<<p_matrix[i][j].netnumb<<")"<<p_matrix[i][j].via<<"   ";
					 }//cout<<endl;
			   }
			   break;


    default: cout<<"you forgot something !!!!!"<<endl;
			 ////cout<<RR<<' '<<CC<<endl;
 }
 //cout<<"--------------------------------"<<endl;
}




///******for checking the deque************///
void printdeque()
{
////cout<<"size of the deque "<<propagatelist.size()<<endl;
for(int i=0;i<propagatelist.size();++i)
{
	////cout<<"x: "<<propagatelist[i].xcoordinate<<" y: "<<propagatelist[i].ycoordinate<<" LeeNumb: "<<propagatelist[i].LeeNumb<<endl;
}
}


////***********Initializing the grid coordinates in layout matrix**********///
void Initializing_layout_coordinates()
{
	for(int i=0;i<layout_RR;i++)
	{
		for(int j=0;j<layout_CC;++j)
		{
			layout_matrix.at(i).at(j).xcoordinate=i;

			layout_matrix.at(i).at(j).ycoordinate=j;
		//////cout<<" Co-ordinates at index "<<i<<"*"<<j<<" x: "<<p_matrix[i][j].xcoordinate<<" y: "<<p_matrix[i][j].ycoordinate<<endl;
		}
	}
}


////***********reading net information from the benchmark file****************////
void InitializeNets()
{
ifstream InputFile(inputfilename[1]);
if(InputFile.is_open())
{
    InputFile>>NumberOfCells;
	InputFile>>NumberOfNets;

	 //////cout<<"number of cells "<<NumberOfCells<<endl;
     //////cout<<"number of nets "<<NumberOfNets<<endl;
net_vector.resize(NumberOfNets);
while(InputFile>>netNum>>c1>>t1>>c2>>t2)
     {
        net_vector.at(netNum-1).net_number=netNum;
        net_vector.at(netNum-1).s_cell=c1-1;
     	net_vector.at(netNum-1).s_terminal=t1-1;
     	net_vector.at(netNum-1).t_cell=c2-1;
     	net_vector.at(netNum-1).t_terminal=t2-1;
     }
}else cout<<"Error opening the file "<<endl;
InputFile.close();
//	for(int i=0;i<net_vector.size();++i)
//	{
//	  cout<<"net numb "<<i<<" s_c: "<< net_vector[i].s_cell<<" s_t: "<<net_vector[i].s_terminal<<" t_c: "<<net_vector[i].t_cell<<" t_t: "<<net_vector[i].t_terminal<<endl;
//	}

}



