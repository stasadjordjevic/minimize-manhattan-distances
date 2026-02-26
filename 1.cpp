#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <algorithm>

using namespace std;

int minimumDistance(vector<vector<int>>& points) {
    int n = points.size();
    
    multiset<int> xs;
    multiset<int> ys;
    multiset<pair<int,int>> points_ms;
    for(int i=0;i<n;i++){
        int tmp = points[i][0];
        points[i][0] = points[i][0]-points[i][1];
        points[i][1] = tmp+points[i][1];
        xs.insert(points[i][0]);
        ys.insert(points[i][1]);
        points_ms.insert(make_pair(points[i][0], points[i][1]));
    }
    int res = INT_MAX;
    for(auto it:points_ms){
        //uklanjamo it i racunamo rez kad ih uklonimo
        auto el = xs.find(it.first);
        if(el!=xs.end())
            xs.erase(el);
        el = ys.find(it.second);
        if(el!=ys.end())
            ys.erase(el);
        int tmp = max(*xs.rbegin()-*xs.begin(), *ys.rbegin()-*ys.begin());
        if(tmp<res) res=tmp;
        xs.insert(it.first);
        ys.insert(it.second);
    } 
    return res;
}

int main(){
    // vector<vector<int>> points = {{3,10},{5,15},{10,2},{4,4}};
    // vector<vector<int>> points = {{1,1},{1,1},{1,1}};
    vector<vector<int>> points = {{3,2},{3,9},{7,10},{4,4},{8,10},{2,7}};
    cout<<minimumDistance(points)<<endl;
    return 0;
}