#include<stdio.h>
#include<stdlib.h>
struct node
{
        int data;
        struct node* next;
};
int main()
{
        struct node *head,*prev,*p,*new,*temp;
        int i=0,n=0,breakpt,count;
        head = NULL;
        printf("enter number of elements:");
        scanf("%d",&n);
        for(i = 0;i<n;i++)
        {
                p = malloc(sizeof(struct node));
                scanf("%d",&p->data);
                p->next = NULL;
                if(head == NULL)
                {
                        head = p;

                }
                else
                {
                        prev->next=p;
                }
                prev= p;
        }
        printf("enter breakpoint");
        scanf("%d",&breakpt);
 for(i = 0;i<n;i++)
        {
                if(head == NULL)
                {
                        printf("null\n");
                        break;
                }
                else
                {
                        if(head->data == breakpt)
                        {
                                temp = malloc(sizeof(struct node));
                                temp->data = head->data;
                                temp->next = head->next;
                                head = NULL;
                                count = i;


                        }
                        else
                        {
                                printf("%d\t",head->data);
                                printf("%ld\n",head->next);
                                head = head->next;

                        }
                }

        }
        head = temp;
        i = count;
        printf("new link list:\n");
        while(i<=n)
        {
                if(temp->next == '\0')
                {
                        printf("%d\t",temp->data);
                        printf("%ld\n",temp->next);
                        exit(1);
                }
                printf("%d\t",temp->data);
                printf("%ld\n",temp->next);
                temp=temp->next;
                i++;
	}
}	
