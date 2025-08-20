#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

struct trinode
{
    trinode *leaves[26]; // for 26 possible char lowercase
    bool end;            // to mark end for word
    trinode()            // constructor
    {
        end = false;
        for (int i = 0; i < 26; i++) // initializing  all to null
        {
            leaves[i] = nullptr;
        }
    }
};



// takes all vector of words and construct tree
trinode *setup(vector<string> &words)
{
    // creating root node for trie
    trinode *root = new trinode();

    // loop through each word in vector
    for (string &w : words)
    {
        // each word starts from root
        trinode *node = root;

        for (char c : w)
        {
            int temp = c - 'a';      // converting chars
            if (!node->leaves[temp]) // checking if it exsits
            {
                node->leaves[temp] = new trinode(); // if not create new one
            }
            node = node->leaves[temp]; // moving to node for next char
        }
        node->end = true; // marking last char of word as end
    }
    return root;
}



// sorting function for lexico order
void sorting(vector<string> &ans)
{
    int size = ans.size();
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size - i - 1; j++)
        {
            if (ans[j] > ans[j + 1])
            {
                string temp = ans[j];
                ans[j] = ans[j + 1];
                ans[j + 1] = temp;
            }
        }
    }
}



// scan word through dict if it is present or not
bool scan(trinode *root, string &word)
{
    trinode *node = root;

    for (char c : word)
    {
        int temp = c - 'a';
        if (!node->leaves[temp]) // checking path exsits for this char
        {
            return 0;
        }
        node = node->leaves[temp];
    }

    // after completing scan  returning if last node is end of valid word
    return node->end;
}



// depth first search to get all words from given node
void dfs(trinode *node, string str, vector<string> &ans)
{

    if (!node) // checking is node is null nothing is there to return
    {
        return;
    }

    if (node->end)
    {
        ans.push_back(str);
    }

    for (int i = 0; i < 26; i++)
    {
        if (node->leaves[i]) // going further
        {
            dfs(node->leaves[i], str + char('a' + i), ans); // all possible words with given prefix
        }
    }
}



// auto complete function
vector<string> autocomplete(trinode *root, string &pre)
{
    trinode *node = root;

    // finding prefix in tri
    for (char c : pre)
    {
        int temp = c - 'a';
        if (!node->leaves[temp])
        {
            return {}; // if pre path doesnot exist
        }

        node = node->leaves[temp]; // moving till we reach at end of pre
    }

    vector<string> ans;
    dfs(node, pre, ans);          // to get all permutation
    sorting(ans);                 // sort in lexico order

    return ans;
}



// levenshtein edit distance
int compute(string &str1, string &str2)
{
    int n = str1.size();
    int m = str2.size();

    // 2d dp for minimum distance between str1 and str2
    vector<vector<int>> dp(n + 1, vector<int>(m + 1));

    for (int i = 0; i < n; i++)
    {
        dp[i][0] = i; // i delete
    }
    for (int j = 0; j < m; j++)
    {
        dp[0][j] = j; // j insert
    }

    // completing dp table entry
    for (int i = 1; i <= n; i++)
    {
        for (int j = 1; j <= m; j++)
        {
            if (str1[i - 1] == str2[j - 1])
            {
                dp[i][j] = dp[i - 1][j - 1];
            }
            else
            {
                int del = dp[i - 1][j];
                int ins = dp[i][j - 1];
                int change = dp[i - 1][j - 1];
                int temp = del;

                // finding minimum
                if (temp > ins)
                {
                    temp = ins;
                }
                if (temp > change)
                {
                    temp = change;
                }
                dp[i][j] = 1 + temp;
            }
        }
    }
    return dp[n][m];
}



// autocorrect function
vector<string> autocorrect(vector<string> &dict, string &query)
{
    vector<string> ans;

    // for each word in dict
    for (int i = 0; i < (int)dict.size(); i++)
    {
        int levdis = compute(query, dict[i]); // calculating levenshtein edit distance
        if (levdis <= 3)
        {
            ans.push_back(dict[i]); // if<=3 store into ans
        }
    }

    sorting(ans); // for lexico order
    return ans;
}



int main()
{
    int n, q;
    cin >> n;
    cin >> q;

    vector<string> dict(n);
    for (int i = 0; i < n; i++)
    {
        cin >> dict[i];
    }

    // building the trie for all words in dict
    trinode *root = setup(dict);

    while (q--)
    {
        int flag;
        string query;
        cin >> flag;
        cin >> query;

        if (flag == 1)
        {
            cout << scan(root, query) << "\n";
        }
        else if (flag == 2)
        {
            vector<string> words = autocomplete(root, query);
            cout << words.size() << endl; // total no. of words found

            for (string &str : words) // for eeach word in vector
            {
                cout << str << endl;
            }
        }

        else if (flag == 3)
        {
            vector<string> ans = autocorrect(dict, query);

            cout << ans.size() << endl; // count of matching words
            for (string &str : ans)
            {
                cout << str << endl;
            }
        }
        else
        {
            cout << "Invalid Input" << endl;
        }
    }
}