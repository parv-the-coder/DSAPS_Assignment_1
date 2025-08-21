#include <iostream>
using namespace std;

const int maxiquery = 100000;        // maxi no. of  queries constrains
const int maxicord = 2 * maxiquery;      // maxi no. of cords (each query give 2 cords)

int cords[maxicord];          // for storing input cords
int diff[maxicord];    // compressed unique cords
int total = 0;        // total different cords
int ndifcord = 0;                 // no. of different cords after compression

// seg tree
int tree[4 * maxicord];        // stores maxi height of each seg
int pending[4 * maxicord];        // pending array for pending updates


// quick-sort
int split(int arr[], int low, int high) 
{
    int pivot = arr[high];
    int i = low - 1;
    for (int j = low; j < high; j++)
    {
        if (arr[j] < pivot) 
        {
            i++;
            int temp = arr[i];
            arr[i]=arr[j];
            arr[j]=temp;
        }
    }

    int temp = arr[i+1];
    arr[i+1]=arr[high];
    arr[high]=temp;
    
    return i+1;
}

void qsort(int arr[], int low, int high) 
{
    if (low < high) {
        int part = split(arr, low, high);
        qsort(arr, low, part - 1);
        qsort(arr, part + 1, high);
    }
}



// compressing different cords
int compdiff(int arr[], int siz, int diff[]) 
{
    
    if (siz == 0) 
    {
        return 0;
    }
    
    int count = 1;
    diff[0] = arr[0];
    
    for (int i = 1; i < siz; i++) 
    {
        if (arr[i] != arr[i-1]) 
        {
            diff[count++] = arr[i];
        }
    }
    
    return count;
}



// return index of cord in diff[]
int indcord(int x)
{
    int low = 0;
    int high = ndifcord - 1;
    while (low <= high) 
    {
        int mid = low + (high - low) / 2;
        if (diff[mid] == x)
        {
            return mid;
        } 
        else if (diff[mid] < x)
        {
            low = mid + 1;
        } 
        else
        {
            high = mid - 1;
        } 
    }
    return -1; // not possible
}



// passing pending values to next node
void pass(int node, int start, int end) 
{
    if (pending[node] > 0) 
    {
        
        // updating the node
        if (pending[node] > tree[node])
        {
            tree[node] = pending[node];
        }

        // passing the update
        if (start != end) 
        {
            if (pending[node] > pending[2*node])
            {
                pending[2*node] = pending[node];
            }
            if (pending[node] > pending[2*node+1])
            {
                pending[2*node+1] = pending[node];
            }
            }
             
        pending[node] = 0;      //no pending updates left
    }
}



// updating from left-right with height
void updating(int node, int start, int end, int left, int right, int height)
 {
    
    pass(node, start, end);

    // distinct
    if (end < left || start > right)
    {
        return;
    } 

    // complete over
    if (left <= start && end <= right) 
    {
        if (height > pending[node])
        {
            pending[node] = height;
        }

        pass(node, start, end);
        return;
    }

    int mid = start + (end-start) / 2;

    updating(2*node, start, mid, left, right, height);
    updating(2*node+1, mid+1, end, left, right, height);

    if (tree[2*node] > tree[2*node + 1])
    {
        tree[node] = tree[2*node];
    }
    else 
    {
        tree[node] = tree[2*node + 1];
    }
}



// query point index (final height after all updates)
int query(int node, int start, int end, int pos) 
{
    pass(node, start, end);

    if (start == end) 
    {
        return tree[node];
    }

    int mid = start + (end-start) / 2;
    if (pos <= mid) 
    {
        return query(2*node, start, mid, pos);
    }
    else 
    {
        return query(2*node+1, mid+1, end, pos);
    }
}



//printing silhouette
void printing()
 {
    int prev = 0;       // keeping track
    for (int i = 0; i < ndifcord-1; i++) 
    {
        int height = query(1, 0, ndifcord-2, i); // get height for i
        if (height != prev) 
        {
            cout << diff[i] << " " << height << "\n";
            prev = height;
        }
    }

    // last cords at 0
    cout << diff[ndifcord-1] << " 0\n";
}



int main() 
{

    ios::sync_with_stdio(false);
    cin.tie(NULL);

    int q;
    cin >> q;   // total no. of queries

    // for storing queries
    int type[maxiquery],l[maxiquery], r[maxiquery], h[maxiquery];

    for (int i = 0; i < q; i++) 
    {
        cin >> type[i];
    
        //getting cords for compression
        if (type[i] == 0)
        {
            cin >> l[i];
            cin >> r[i];
            cin >> h[i];
            cords[total++] = l[i];
            cords[total++] = r[i];
        }

    }

    // compressing cords by remv duplicate and store in diff
    qsort(cords, 0, total-1);
    ndifcord = compdiff(cords, total, diff);

    // computing query
    for (int i = 0; i < q; i++) 
    {

        if (type[i] == 0)
        {
            int left = indcord(l[i]);
            int right = indcord(r[i]) - 1;

            updating(1, 0, ndifcord-2, left, right, h[i]);
        } 

        else 
        {
            cout<<endl;
            printing();
            cout<<endl;
        }

    }
}
