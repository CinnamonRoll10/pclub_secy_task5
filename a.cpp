#include <bits/stdc++.h>
using namespace std;
#define INF (int)1e18
#define f first
#define s second

mt19937_64 RNG(chrono::steady_clock::now().time_since_epoch().count());

const int A = 40; 
const int F = 420; 
const int P = 4e4; 
const int T = 2e5; 
int dep[F], arr[F], cap[F], dept[F], arrt[F], fr[F];
bool gone[F];
vector <int> can, pass[F];
int flight[P];
int f = 0, p = 0;
int timer;
int affected, alloc;
double avglay;
double avgdiff;

void takeinput(){
    ifstream file("flights.csv");
    string line;
    getline(file, line);
    while (getline(file, line)){
        string x = "";
        vector <int> b;
        for (auto y : line){
            if (y == ','){
                b.push_back(stoi(x));
                x = "";
            } else {
                x += y;
            }
        }
        b.push_back(stoi(x));
        assert(b.size() == 6);

        dep[f] = b[1];
        arr[f] = b[2];
        cap[f] = b[3];
        dept[f] = b[4];
        arrt[f] = b[5];
        f++;
    }
 //   cout << f << "\n";

    file.close();

    ifstream file1("canceled.csv");
    getline(file1, line);
    while (getline(file1, line)){
        can.push_back(stoi(line));
        gone[can.back()] = true;
    }
    file1.close();

   // cout << f << "\n";

    ifstream file2("passengers.csv");
    getline(file2, line);
    while (getline(file2, line)){
        string x = "";
        vector <int> b;
        for (auto y : line){
            if (y == ','){
                b.push_back(stoi(x));
                x = "";
            } else {
                x += y;
            }
        }
        b.push_back(stoi(x));
        assert(b.size() == 2);

        pass[b[1]].push_back(p);
        flight[p] = b[1];
        p++;
      //  cout << f << " ";
    }
    file2.close();
  //  cout << f << "\n";
}

int calc(int x, vector<int> v){
    // For every passenger, we will keep a total value of 500 
    // One layover gets a value in [400, 500] depending on length of flight + difference from original timing 
    // 2 layovers gets a value in [275, 425]
    // 3 layovers gets a value in [150, 350]

    // within that range, half points are for length of flight
    // half for difference from original timing

//    cout << "Doing\n";

    int add, range;
    int ans = 0;
    if (v.size() == 1){
            add = 400;
            range = 100;
        } else if (v.size() == 2){
            add = 275;
            range = 150;
        } else {
            add = 150;
            range = 200;
        }

      //  cout << add << " " << range << "\n";

        ans += add;
        range /= 2;

        int ilen = arrt[x] - dept[x];
        int flen = arrt[v.back()] - dept[v[0]];
        assert(ilen > 0 && flen > 0);

        if (flen <= ilen){
            ans += range;
        } else {
            ans += round((double)range * ilen / flen);
        }

        //cout << ans << " ";

        int iarr = arrt[x];
        int farr = arrt[v.back()];

        int diff = abs(farr - iarr);
        if (farr < iarr){
            diff /= 2; // if reached earlier we prefer it to later
        }

        // difference can be around 200000
        // diff = 200000 => 0
        // diff = 0 => range 
        diff = 200000 - diff;
        ans += round((double)range * diff / 200000);
       // cout << ans << "\n";
        return ans;
}

int ranking(vector <int> a, vector<vector<int>> fl){
    // a is list of flights of the passengers reallocated 
    // fl is list of 
    int ans = 0;

    int sz = a.size();
    assert(fl.size() == sz);
    for (int i = 0; i < sz; i++){
        int x = a[i];
        auto v = fl[i];

        ans += calc(x, v);
    }

    return ans;
}

void Solve() 
{
    takeinput();

    for (int i = 0; i < f; i++){
        if (!gone[i]){
            fr[i] = cap[i] - pass[i].size();
        }
    }

    vector<int> na;
    vector<vector<int>> nb;
    map <int, int> left;

    vector<pair<int, pair<int, vector<int>>>> vec;
    int initial = 0, fin = 0;
    int v1 = 0, v2 = 0, v3 = 0;

    for (auto x : can){
        vector <int> nfr(f);
        for (int i = 0; i < f; i++) nfr[i] = fr[i];

        int move = pass[x].size();
        initial += move;

        for (int i = 0; i < f; i++){
            if (!gone[i] && dep[i] == dep[x] && arr[i] == arr[x]){
                vec.push_back({calc(x, {i}), {x, {i}}});

                int take = min(move, fr[i]);
                fr[i] -= take;
                move -= take;
                v1++;
            }
        }

        for (int i = 0; i < f; i++) for (int j = 0; j < f; j++){
            if (!gone[i] && !gone[j] && dep[i] == dep[x] && arr[i] == dep[j] && arr[j] == arr[x] && dept[j] > arrt[i] + 3600){
                vec.push_back({calc(x, {i, j}), {x, {i, j}}});

                int take = min({move, fr[i], fr[j]});
                fr[i] -= take;
                fr[j] -= take;
                move -= take;
                v2++;
            }
        }

        for (int i = 0; i < f; i++) for (int j = 0; j < f; j++) for (int k = 0; k < f; k++){
            if (!gone[i] && !gone[j] && !gone[k] && dep[i] == dep[x] && arr[i] == dep[j] && arr[j] == dep[k] && arr[k] == arr[x] && dept[j] > arrt[i] + 3600 && dept[k] > arrt[j] + 3600){
                vec.push_back({calc(x, {i, j, k}), {x, {i, j, k}}});

                int take = min({move, fr[i], fr[j], fr[k]});
                fr[i] -= take;
                fr[j] -= take;
                fr[k] -= take;
                move -= take;
                v3++;
            }
        }

        fin += move;

        for (int i = 0; i < f; i++){
            fr[i] = nfr[i];
        }


        left[x] = pass[x].size();
    }

  //  cout << v1 << " " << v2 << " " << v3 << "\n";

    // cout << initial << " " << fin << "\n";

    sort(vec.begin(), vec.end());
    reverse(vec.begin(), vec.end());

    // for (auto x : vec){
    //     cout << x.first << " " << x.second.second.size() << "\n";
    // }
    // exit(0);

    vector <int> a;
    vector<vector<int>> b;

    int ans = 0;
    for (int i = 0; i < 10000; i++){
        //shuffle(vec.begin(), vec.end(), RNG);
        sort(vec.begin(), vec.end());
        reverse(vec.begin(), vec.end());

        for (int j = 0; j < 20; j++){
            int i1 = RNG() % (int)(vec.size());
            int i2 = RNG() % (int)(vec.size());

            swap(vec[i1], vec[i2]);
        }

        map <int, int> ll;
        vector <int> nfr(f);
        ll = left;
        for (int j = 0; j < f; j++) nfr[j] = fr[j];

        vector <int> na;
        vector <vector<int>> nb;

        for (auto x : vec){
            int take = left[x.second.first];
            for (auto y : x.second.second){
                take = min(take, fr[y]);
            }

            left[x.second.first] -= take;
            for (auto y : x.second.second){
                fr[y] -= take;
            }

            while (take--){
                na.push_back(x.second.first);
                nb.push_back(x.second.second);
            }
        }

        int v = ranking(na, nb);
        if (v > ans){
            ans = v;
            a = na;
            b = nb;
        }

       // cout << v << "\n";

        left = ll;
        for (int j = 0; j < f; j++){
            fr[j] = nfr[j];
        }
    }

  //   cout << ans << "\n";

    // for (auto x : b){
    //     for (auto y : x){
    //         cout << y << " ";
    //     }
    //     cout << "\n";
    // }

    // cout << ans << "\n";

    affected = initial;
    alloc = a.size();
    avglay = 0;
    for (auto x : b){
        avglay += x.size() - 1;
    }
    avglay /= alloc;
    for (int i = 0; i < alloc; i++){
        int ac = arrt[a[i]];
        int nw = arrt[b[i].back()];

        avgdiff += abs(ac - nw);
    }

    avgdiff /= alloc;

    ofstream file2("allot.csv");
    vector <int> result[P];
    for (int i = 0; i < alloc; i++){
        result[pass[a[i]].back()] = b[i];
        pass[a[i]].pop_back();
    }

    for (int i = 0; i < p; i++){
        bool aff = false;
        for (auto x : can) if (flight[i] == x){
            aff = true;
        }

        if (aff){
            file2 << i << ",";
            file2 << result[i].size();
            for (auto y : result[i]){
                file2 << "," << y;
            }
            file2 << "\n";
        }
    }
}

int32_t main() 
{
    auto begin = std::chrono::high_resolution_clock::now();
    ios_base::sync_with_stdio(0);
    cin.tie(0);
    int t = 1;
    // freopen("in",  "r", stdin);
    // freopen("out", "w", stdout);
    
  //  cin >> t;
    for(int i = 1; i <= t; i++) 
    {
        //cout << "Case #" << i << ": ";
        Solve();
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);

    timer = round(elapsed.count() * 1e-6);

    ofstream file("stats.csv");
    file << fixed << setprecision(3);
    file << "Affected,Reallocated,AvgLay,TimeDiff,SolTime\n";
    file << affected << "," << alloc << "," << avglay << "," << avgdiff << "," << timer << "\n";
    file.close();

    // cerr << "Time measured: " << elapsed.count() * 1e-9 << " seconds.\n"; 
    return 0;
}