class BinarySearch {
public:
    int getPos(vector<int> A, int n, int val) {        
        int lower = -1;
        int high = n - 1;
        while (lower + 1 != high)
        {
            int middle = (lower + high)/2;
            if(A[middle] < val)
            {
                lower = middle;
            }
            else
            {
                high = middle;
            }
        }
        
        if(A[high] != val)
            return -1;
        else
            return high;
    }
};
