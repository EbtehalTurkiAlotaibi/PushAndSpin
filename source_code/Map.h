/* iPath: A C++ Library of Intelligent Global Path Planners for Mobile Robots with ROS Integration.

 * Website: http://www.iroboapp.org/index.php?title=IPath
 * Contact:
 *
 * Copyright (c) 2014
 * Owners: Al-Imam University/King AbdulAziz Center for Science and Technology (KACST)/Prince Sultan University
 * All rights reserved.
 *
 * License Type: GNU GPL
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MAP_H
#define MAP_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>
using namespace std;

/**
* @class Map
* @brief A class that represents a grid environment as a two dimensional matrix
*/
class Map {

public:

    /**
    * @brief default constructor to initialize map
    */
    Map(int w=0, int h=0, float res=0.0);
    /**
    * @brief four-argument constructor o initialize map
    */
    Map(int w, int h, float res, char **mapMatrix, int obsSize, float obsRatio);
    //constructor that loads a map from a file
    // Map(ifstream mpgMapFile);
    /**
    * @brief Copy constructor for the map
    */
    Map (Map* map);


    //Define Mutators
    void setWidth(int w);
    void setHeight(int h);
    void setResolution (float res);
    void setMapLayout(char** mapMatrix);
    void setObstacleSize (int obsSize);
    void setObstacleRatio(float obsRatio);

    //Define Accessors
    int getWidth();
    int getHeight();
    float getResolution();
    char **getMapLayout();
    int getObstacleSize();
    float getObstacleRatio();

    /**
    * @brief it is used to get the index of the cell to be used in Path.
    * @param i
    * @param j
    * @return cell index
    */
    int getCellIndex(int i,int j);
    /**
    * @brief get the row ID from cell index
    * @param index
    * @return cell row ID
    */
    int getCellRowID(int index);
    /**
    * @brief get colunm ID from cell index
    * @param index
    * @return cell col ID
    */
    int getCellColID(int index);

    /**
    * @brief it is used to read the MapLayout from a file
    * @param path_name
    * @param file_name
    */
    void importMapLayout(string path_name, const char* file_name);
    /**
    * @brief it is used to write the MapLayout in a file
    * @param file_name
    * @param mapMatrix
    */
    void exportMapLayout(const char* file_name, int** mapMatrix);

protected:
  int width; //!<  Map width [cells]
  int height;//!<  Map height [cells]
  float resolution; //!< The map resolution [m/cell]
  char** mapLayout; //!<  the map data structure that contains all cells of the environment
  int obstacleSize; //!< The size of the obstacles
  float obstacleRatio; //!< The ratio of obstacles in the map
};

#endif
