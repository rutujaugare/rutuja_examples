#include<stdio.h>
void main()
{
        int n,arr[50],i=0,k=0,new[50];
        printf("enter a number of elements\n");
        scanf("%d",&n);
        printf("enter array elements\n");
        for(i = 0; i< n;i++)
        {
                scanf("%d",&arr[i]);
        }
        for(i = 0;i<n;i++)
        {
                if(arr[i] == 0)
                {
                        new[k] = arr[i] ;
                        k++;
                }
        }
        for(i = 0;i<n;i++)
        {
                if(arr[i] != 0)
                {
                        new[k] = arr[i];
                        k++;
                }
        }
        printf("The new array is:\n");
        for(i=0;i<n;i++)
        {
                printf("%d\n",new[i]);
        }
}

