#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <algorithm>

using namespace std;

int manhattanDistance(const vector<vector<int>>& points,int i,int j){
    return abs(points[i][0]-points[j][0])+abs(points[i][1]-points[j][1]);
}

pair<int,int> extremePoints(const vector<vector<int>>& points, int skipIndex){
    int n = points.size();
    
    int max_sum=INT_MIN;
    int min_sum=INT_MAX;
    int max_diff=INT_MIN;
    int min_diff=INT_MAX;
    int max_sum_idx=-1;
    int min_sum_idx=-1;
    int max_diff_idx=-1;
    int min_diff_idx=-1;

    for(int i=0;i<n;i++){
        if(i==skipIndex) continue;
        int sum_i = points[i][0]+points[i][1];
        int diff_i = points[i][0]-points[i][1];
        if(sum_i>max_sum){
            max_sum = sum_i;
            max_sum_idx=i;
        }
        if(sum_i<min_sum){
            min_sum = sum_i;
            min_sum_idx=i;
        }
        if(diff_i>max_diff){
            max_diff = diff_i;
            max_diff_idx=i;
        }
        if(diff_i<min_diff){
            min_diff = diff_i;
            min_diff_idx=i;
        }
    }
    int r1 = max_sum-min_sum; //result1
    int r2 = max_diff-min_diff; //result2
    int point1 = max_sum_idx;
    int point2 = min_sum_idx;
    if(r2>r1){
        point1=max_diff_idx;
        point2=min_diff_idx;
    }
    return {point1,point2};
}


int minimumDistance(const vector<vector<int>>& points) {
    int n = points.size();
    auto res = extremePoints(points,-1);
    int p1_idx=res.first;
    int p2_idx=res.second;

    // nasli smo max rastojanje i tacke koje ucestvuju u njemu
    // sad gledamo uklanjanjem koje od njih dobijamo bolji (manji) rezultat
    auto without_p1=extremePoints(points,p1_idx);
    auto without_p2=extremePoints(points,p2_idx);

    int res1 = manhattanDistance(points, without_p1.first, without_p1.second);
    int res2 = manhattanDistance(points, without_p2.first, without_p2.second);
    return min(res1,res2);
}

int main(){
    vector<vector<int>> points = {{3,10},{5,15},{10,2},{4,4}};
    // vector<vector<int>> points = {{1,1},{1,1},{1,1}};
    // vector<vector<int>> points = {{3,2},{3,9},{7,10},{4,4},{8,10},{2,7}};
    cout<<minimumDistance(points)<<endl;
    return 0;
}