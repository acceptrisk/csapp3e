void insertsort(int array[], int n)
{
    for(int i = 1; i < n; ++i)
    {
       int temp = array[i];
       for(int j = i; j > 0 && array[j-1] > temp;--j)
       {
           array[j] = array[j-1];
       }
       array[j] = temp;
    }
}
