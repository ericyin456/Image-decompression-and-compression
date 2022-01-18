#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class ImageCompression{
    public:
    int numRows, numCols, minVal, maxVal, newMinVal, newMaxVal;
    int** ZFAry;
    int** skeletonAry;

    ImageCompression(int _row, int _col, int _min, int _max){
        numRows = _row;
        numCols = _col;
        minVal = _min;
        maxVal = _max;
        newMinVal = _min;
        newMaxVal = _max;

        ZFAry = new int*[numRows+2];
        for(int i = 0; i < numRows+2; i++){
            ZFAry[i] = new int[numCols+2];
        }
        skeletonAry = new int*[numRows+2];
        for(int i = 0; i < numRows+2; i++){
            skeletonAry[i] = new int[numCols+2];
        }
    }

    void setZero(int** ary){
        for(int i = 0; i  < numRows+2; i++){
            for(int j = 0; j < numCols+2; j++){
                ary[i][j] = 0;
            }
        }
    }
    void loadImage(ifstream& inFile, int** ary){
        if(inFile.is_open()){
            for(int i = 1; i <= numRows; i++){
                for(int j = 1; j <= numCols; j++){
                    inFile >> ary[i][j];
                }
            }
        }
    }
    void loadSkeleton(ifstream& skeletoninFile, int** ary){
        int row, col, min, max;
        skeletoninFile >> row;
        skeletoninFile >> col; 
        skeletoninFile >> min;
        skeletoninFile >> max;

        while(!skeletoninFile.eof()){
            int tempR;
            int tempC;
            int tempVal;
            skeletoninFile >> tempR;
            skeletoninFile >> tempC;
            skeletoninFile >> tempVal;
            ary[tempR][tempC] = tempVal;
        }
    }
    void compute8Distance(int** _ZFAry, ofstream& outFile1){
        firstPass8Distance(_ZFAry);
        reformatPrettyprint(_ZFAry, outFile1, 1);
        secondPass8Distance(_ZFAry);
        reformatPrettyprint(_ZFAry, outFile1, 2);
    }
    void firstPass8Distance(int** _ZFAry){
        for(int i = 1; i <= numRows; i++){
            for(int j = 1; j <= numCols; j++){
                if(_ZFAry[i][j] > 0){
                    _ZFAry[i][j] = minOfNeighborFirstPass(i,j, ZFAry) + 1;
                    //change maxVal
                    if(newMaxVal < _ZFAry[i][j])
                        newMaxVal = _ZFAry[i][j];                    
                }
            }
        }
    }
    int minOfNeighborFirstPass(int row, int col, int** _ZFAry){
        int min = _ZFAry[row][col-1];

        for(int i = -1; i <= 1; i++){
            if(min > _ZFAry[row-1][col+i])
                min = _ZFAry[row-1][col+i];
        }

        return min;
    }
    void secondPass8Distance(int** _ZFAry){
        newMaxVal = maxVal;
        for(int i = numRows+1; i > 0; i--){
            for(int j = numCols+1; j > 0; j--){
                if(_ZFAry[i][j] > 0)
                    _ZFAry[i][j] = minOfNeighborSecondPass(i, j, ZFAry);
                //change maxVal
                if(newMaxVal < _ZFAry[i][j])
                    newMaxVal = _ZFAry[i][j];
            }
        }
    }
    int minOfNeighborSecondPass(int row, int col, int** _ZFAry){
        int min = _ZFAry[row][col];
        
        if(min > _ZFAry[row][col+1]+1)
            min = _ZFAry[row][col+1]+1;

        for(int i = -1; i <= 1; i++){
            if(min > _ZFAry[row+1][col+i]+1)
                min = _ZFAry[row+1][col+i]+1;
        }
        return min;
    }
    void imageCompression(int** _ZFAry, int** _skeletonAry, ofstream& skeletonFile, ofstream& outFile1){
        computeLocalMaxima(_ZFAry, _skeletonAry);
        reformatPrettyprint(_skeletonAry, outFile1, 3);
        extractSkeleton(_skeletonAry, skeletonFile);
    }
    bool isLocalMaxima(int row, int col, int** _ZFAry){
        for(int i = -1; i <= 1; i++){
            for(int j = -1; j <= 1; j++){
                if(i == 0 && j == 0) continue;
                if(_ZFAry[row][col] < _ZFAry[row+i][col+j]) return false;
            }
        }
        return true;
    }
    void computeLocalMaxima(int** _ZFAry, int** _skeletonAry){
        for(int i = 1; i <= numRows; i++){
            for(int j = 1; j <= numCols; j++){
                if(_ZFAry[i][j] > 0){
                    if(isLocalMaxima(i, j, _ZFAry))
                        _skeletonAry[i][j] = _ZFAry[i][j];
                    else
                        _skeletonAry[i][j] = 0;
                }
                
            }
        }
    }
    void extractSkeleton(int** _skeletonAry, ofstream& skeletonFile){
        skeletonFile << numRows << " " << numCols << " " << newMinVal << " " << newMaxVal << endl;

        for(int i = 1; i <= numRows; i++){
            for(int j = 1; j <= numCols; j++){
                if(_skeletonAry[i][j] > 0){
                    skeletonFile << i << " " << j << " " << _skeletonAry[i][j] << endl;
                }
            }
        }
    }
    void imageDeCompression(int** ary, ofstream& outFile1){
        firstPassExpension(ary);
        reformatPrettyprint(ary, outFile1, 4);
        secondPassExpension(ary);
        reformatPrettyprint(ary, outFile1, 5);
    }
    
    void firstPassExpension(int** ary){
        for(int i = 1; i <= numRows; i++){
            for(int j = 1; j<= numCols; j++){
                if(ary[i][j] == 0)
                    ary[i][j] = maxOfNeighborFirstPass(i, j, ary);
            }
        }
    }
    int maxOfNeighborFirstPass(int row, int col, int** ary){
        int max = 0;
        for(int i = -1; i <= 1; i++){
            for(int j = -1; j <= 1; j++){
                if(i == 0 && j == 0) continue;
                if(max < ary[row+i][col+j]-1){
                    max = ary[row+i][col+j]-1;
                }
            }
        }
        return max;
    }
    void secondPassExpension(int** ary){
        for(int i = numRows; i > 0; i--){
            for(int j = numCols; j > 0; j--){
                ary[i][j] = maxOfNeighborSecondPass(i, j, ary);
            }
        }
    }
    int maxOfNeighborSecondPass(int row, int col, int** ary){
        int max = ary[row][col];
        for(int i = -1; i <= 1; i++){
            for(int j = -1; j <= 1; j++){
                if(i == 0 && j == 0) continue;
                if(max < ary[row+i][col+j]-1){
                    max = ary[row+i][col+j]-1;
                }
            }
        }
        return max;
    }
    void threshold(int** ary, ofstream& deCompressFile){
        for(int i = 1; i < numRows; i++){
            for( int j = 1; j < numCols; j++){
                if(ary[i][j] >= 1)
                    deCompressFile << "1 ";
                else
                    deCompressFile << "0 ";
            }
            deCompressFile << endl;
        }
    }
    void reformatPrettyprint(int** ary, ofstream& outFile1, int x){
        if( x == 1){
            outFile1 << "8 Distance First Pass" << endl;
        }
        else if( x == 2){
            outFile1 << "8 Distance Second Pass" << endl;
        }
        else if (x == 3){
            outFile1 << "Skeleton Array" << endl;
        }
        else if (x == 4){
            outFile1 << "First Pass Expansion" << endl;
        }
        else if (x == 5){
            outFile1 << "Second Pass Expansion" << endl;
        }

        outFile1 << numRows << " " << numCols << " " << newMinVal << " " << newMaxVal << endl;
        
        string str = to_string(maxVal);
        int width = str.length();

        for(int i = 1; i <= numRows; i++){
            for(int j = 1; j <= numCols; j++){
                outFile1 << ary[i][j] << " ";
                str = ary[i][j];
                int WW = str.length();
                while(WW < width){
                    outFile1 << " ";
                    WW++;
                }
            }
            outFile1 << endl;
        }
        outFile1 << endl;
    }

};

int main(int argc, char* argv[]){
    string arg = argv[1];
    ifstream inFile(argv[1]);
    ofstream outFile1(argv[2]);
    ofstream skeletonFile(arg+"_skeleton.txt");
    ofstream deCompressFile(arg+"_deCompressed.txt");

    int row, col, min, max;
    if(inFile.is_open()){
        inFile >> row;
        inFile >> col;
        inFile >> min;
        inFile >> max;
    }
    
    ImageCompression img(row, col, min, max);
    img.setZero(img.ZFAry);
    img.setZero(img.skeletonAry);
    img.loadImage(inFile, img.ZFAry);
    img.compute8Distance(img.ZFAry, outFile1);
    img.imageCompression(img.ZFAry, img.skeletonAry, skeletonFile, outFile1);
    skeletonFile.close();

    ifstream skeletoninFile(arg+"_skeleton.txt");
    img.setZero(img.ZFAry);
    img.loadSkeleton(skeletoninFile, img.ZFAry);
    img.imageDeCompression(img.ZFAry, outFile1);

    deCompressFile << img.numRows << " " << img.numCols << " " << img.newMinVal << " " << img.newMaxVal << endl;
    img.threshold(img.ZFAry, deCompressFile);

    inFile.close();
    outFile1.close();
    skeletonFile.close();
    deCompressFile.close();

    return 0;
}