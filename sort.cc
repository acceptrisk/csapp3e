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

#pragma once

/*!
@brief double linked list struct and functions definition
*/

/*!
@brief list_node struct
*/
struct list_node
{
    struct list_node *prev, *next;
};

/*!
@brief init list
@param[in] list  pointer to a double linked list
@return pointer to the double linked list
*/
static inline struct list_node* list_init(struct list_node *list)
{
    list->next = list->prev = list;
    return list;
}

/*!
@brief insert a node into a double linked list before specified position
@param[in] before  pointer to some node that a new one will be inserted before it
@param[in] node  node pointer to be inserted
*/
static inline void list_insert(struct list_node *before, struct list_node *node)
{
    struct list_node *prev = before->prev;    

    node->next = before;
    node->prev = prev;

    before->prev = node;
    prev->next = node;
}

/*!
@brief remove a node from a double linked list
@param[in] node  node pointer to be removed
@return  list  double linked list
*/
static inline struct list_node* list_remove_safe(struct list_node *node)
{
    struct list_node *prev, *next;

    prev = node->prev;
    next = node->next;

    prev->next = next;
    next->prev = prev;

    node->next = node->prev = node;
    return node;
}

/*!
@brief check if a double linked list is empty
@param[in] list  pointer to a double linked list
@return whether a list is empty
*/
static inline bool list_empty(struct list_node *list)
{
    return list->next == list;
}


int binary_search(int arr[], int num, int value)
{
    if (num <= 0) return -1;
    int lower = -1;
    int upper = num - 1;
    while (lower + 1 != upper)
    {
        int mid = (lower + upper) / 2;
        if (arr[mid] < value)
        {
            lower = mid;
        } 
        else
        {
            upper = mid;
        }
    }

    if (arr[upper] != value)
    {
        return -1;
    } 
    else
    {
        return upper;
    }
}


void insertion_sort(int arr[], int num)
{    
    for (auto i=1;i < num;++i)
    {
        auto temp = arr[i];
        for (auto j = i; j > 0 && arr[j - 1] > temp; --j)
        {
            arr[j] = arr[j - 1];
        }
        arr[j] = temp;
    }
}

void quick_sort(int arr[], int lower, int upper)
{
    if (lower >= upper) return;
    int pivot = arr[lower];
    int index = lower;
    for (auto i = lower+1; i <= upper; ++i)
    {
        if (arr[i] < pivot)
        {
            swap(arr[++index], arr[i]);
        }
    }
    swap(arr[lower], arr[index]);
    quick_sort(arr, lower, index - 1);
    quick_sort(arr, index + 1, upper);
}

void quick_sort2(int arr[], int lower, int upper)
{
    if (lower >= upper) return;
    int num = rand() % (upper - lower + 1) + lower;
    swap(arr, lower, num);

    int pivot = arr[lower];

    int i = lower;
    int j = upper + 1;
    while (1)
    {
        do
        {
            ++i;
        } while (i < j && arr[i] < pivot);

        while (arr[--j] > pivot)
        {
            ;
        }

        if (i > j)
        {
            break;
        }

        swap(arr, i, j);
    }
    swap(arr, lower, j);
    quick_sort(arr, lower, j - 1);
    quick_sort(arr, j + 1, upper);
}





