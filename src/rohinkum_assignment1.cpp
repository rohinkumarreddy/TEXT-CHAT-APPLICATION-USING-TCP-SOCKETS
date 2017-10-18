/**
 * @rohinkum_assignment1
 * @author  rohinkumarreddy <rohinkum@buffalo.edu>
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * This contains the main function. Add further description here....
 */
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "tcp_server_func.h"
#include "tcp_client_func.h"

#include "../include/global.h"
#include "../include/logger.h"

using namespace std;

/**
 * main function
 *
 * @param  argc Number of arguments
 * @param  argv The argument list
 * @return 0 EXIT_SUCCESS
 */
int main(int argc, char **argv)
{
	/*Init. Logger*/
	cse4589_init_log(argv[2]);

	/* Clear LOGFILE*/
    fclose(fopen(LOGFILE, "w"));

	/*Start Here*/
	int port1;
 	if(argc!=3)
 	{
  		cerr<<"[ERROR:INVALID ARGUMENTS]"<<endl;
 	}
 	else if(argc>2)
 	{
 	 	if(argv[2]!=NULL)
 	 	{
 		  	port1=atoi(argv[2]);//collecting port value from argument
 	 	}
  		if(**(argv+1)=='s')
  		{
        	                 /*cout<<"main success!"<<endl;*/
 		  	tcp_server_func(port1);
  		}
  		else if(**(argv+1)=='c')
  		{
 		  	tcp_client_func(port1);
  		}
 	}
	return 0;
}
