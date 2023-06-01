#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Structure to store menu item details
typedef struct menu_item
{
    int item_no;
    char name[50];
    float price;
} MenuItem;

// Structure to store order details
typedef struct order
{
    int orderNumber;
    char customerName[50];
    MenuItem *menuItems;
    int numItems;
    float totalCost;
    struct order *left;
    struct order *right;
} OrderNode;

// Function prototypes
void displayMenu(MenuItem *menu, int numItems);
void placeOrder(OrderNode **root, MenuItem *menu, int numItems, int orderNumber, char customerName[]);
void addItem(OrderNode *order, char itemName[], int quantity, MenuItem *menu, int numItems);
void displayOrder(OrderNode *order);
void deleteOrder(OrderNode **root, int orderNumber);
void searchOrder(OrderNode *root, int orderNumber);
void saveOrdersToFile(OrderNode *root, FILE *file);
void loadOrdersFromFile(OrderNode **root, FILE *file);
void freeOrders(OrderNode *root);

// Function to display the menu
void displayMenu(MenuItem *menu, int numItems)
{
    printf("Menu:\n");
    for (int i = 0; i < numItems; i++)
    {
        printf("%d. %s - $%.2f\n", i + 1, menu[i].name, menu[i].price);
    }
}

// Function to place an order
void placeOrder(OrderNode **root, MenuItem *menu, int numItems, int orderNumber, char customerName[])
{
    OrderNode *newOrder = (OrderNode *)malloc(sizeof(OrderNode));
    newOrder->orderNumber = orderNumber;
    strcpy(newOrder->customerName, customerName);
    newOrder->menuItems = NULL;
    newOrder->numItems = 0;
    newOrder->totalCost = 0;
    newOrder->left = NULL;
    newOrder->right = NULL;

    int k;
    char d;

    do
    {
        printf("enter the menu item no u want to order: ");
        scanf("%d", &k);
        char itemName[50];
        int quantity;
        printf("Enter quantity: ");
        scanf("%d", &quantity);
        addItem(newOrder, menu[k].name, quantity, menu, numItems);
        printf("wants to add more item? (y/n): ");
        scanf("%s", &d);
        if (d == 'y' || d == 'Y')
        {
            k = -1;
        }
        else if (d == 'n' || d == 'N')
        {
            k = 1;
        }
    } while (k < 0);

    *root = newOrder;
}

// Function to add an item to an order
void addItem(OrderNode *order, char itemName[], int quantity, MenuItem *menu, int numItems)
{
    int itemIndex = -1;
    for (int i = 0; i < numItems; i++)
    {
        if (strcmp(itemName, menu[i].name) == 0)
        {
            itemIndex = i;
            break;
        }
    }

    if (itemIndex == -1)
    {
        printf("Item not found in the menu.\n");
        return;
    }

    order->menuItems = (MenuItem *)realloc(order->menuItems, (order->numItems + 1) * sizeof(MenuItem));
    strcpy(order->menuItems[order->numItems].name, itemName);
    order->menuItems[order->numItems].price = menu[itemIndex].price;
    order->numItems++;
    order->totalCost += menu[itemIndex].price * quantity;
}

// Function to display an order
void displayOrder(OrderNode *order)
{
    printf("Order Number: %d\n", order->orderNumber);
    printf("Customer Name: %s\n", order->customerName);
    printf("Items:\n");
    for (int i = 0; i < order->numItems; i++)
    {
        printf("%s - $%.2f\n", order->menuItems[i].name, order->menuItems[i].price);
    }
    printf("Total Cost: $%.2f\n", order->totalCost);
}

// Function to delete an order
void deleteOrder(OrderNode **root, int orderNumber)
{
    if (*root == NULL)
    {
        printf("Order not found.\n");
        return;
    }

    if (orderNumber < (*root)->orderNumber)
    {
        deleteOrder(&(*root)->left, orderNumber);
    }
    else if (orderNumber > (*root)->orderNumber)
    {
        deleteOrder(&(*root)->right, orderNumber);
    }
    else
    {
        if ((*root)->left == NULL && (*root)->right == NULL)
        {
            free((*root)->menuItems);
            free(*root);
            *root = NULL;
        }
        else if ((*root)->left == NULL)
        {
            OrderNode *temp = *root;
            *root = (*root)->right;
            free(temp->menuItems);
            free(temp);
        }
        else if ((*root)->right == NULL)
        {
            OrderNode *temp = *root;
            *root = (*root)->left;
            free(temp->menuItems);
            free(temp);
        }
        else
        {
            OrderNode *successor = (*root)->right;
            while (successor->left != NULL)
            {
                successor = successor->left;
            }
            (*root)->orderNumber = successor->orderNumber;
            strcpy((*root)->customerName, successor->customerName);
            (*root)->menuItems = successor->menuItems;
            (*root)->numItems = successor->numItems;
            (*root)->totalCost = successor->totalCost;
            deleteOrder(&(*root)->right, successor->orderNumber);
        }
        printf("Order deleted successfully.\n");
        printf("Place another order if you are not satisfied...!!");
    }
}

// Function to search for an order
void searchOrder(OrderNode *root, int orderNumber)
{
    if (root == NULL)
    {
        printf("Order not found.\n");
        return;
    }
    if (orderNumber < root->orderNumber)
    {
        searchOrder(root->left, orderNumber);
    }
    else if (orderNumber > root->orderNumber)
    {
        searchOrder(root->right, orderNumber);
    }
    else
    {
        displayOrder(root);
    }
}

// Function to save orders to file
void saveOrdersToFile(OrderNode *root, FILE *file)
{
    if (root == NULL)
    {
        return;
    }
    saveOrdersToFile(root->left, file);

    fprintf(file, "Order Number: %d\n", root->orderNumber);
    fprintf(file, "Customer Name: %s\n", root->customerName);
    fprintf(file, "Items:\n");
    for (int i = 0; i < root->numItems; i++)
    {
        fprintf(file, "%s - $%.2f\n", root->menuItems[i].name, root->menuItems[i].price);
    }
    fprintf(file, "Total Cost: $%.2f\n", root->totalCost);
    fprintf(file, "\n");

    saveOrdersToFile(root->right, file);
}

// Function to load orders from file
void loadOrdersFromFile(OrderNode **root, FILE *file)
{
    while (!feof(file))
    {
        OrderNode *newOrder = (OrderNode *)malloc(sizeof(OrderNode));
        newOrder->menuItems = NULL;
        newOrder->left = NULL;
        newOrder->right = NULL;
        fscanf(file, "Order Number: %d\n", &newOrder->orderNumber);
        fscanf(file, "Customer Name: %[^\n]\n", newOrder->customerName);
        char buffer[100];
        fgets(buffer, sizeof(buffer), file); // Read the "Items:" line

        while (fgets(buffer, sizeof(buffer), file))
        {
            if (strcmp(buffer, "\n") == 0)
            {
                break;
            }

            MenuItem item;
            sscanf(buffer, "%[^-] - $%f\n", item.name, &item.price);

            newOrder->menuItems = (MenuItem *)realloc(newOrder->menuItems, (newOrder->numItems + 1) * sizeof(MenuItem));
            strcpy(newOrder->menuItems[newOrder->numItems].name, item.name);
            newOrder->menuItems[newOrder->numItems].price = item.price;
            newOrder->numItems++;
        }

        fscanf(file, "Total Cost: $%f\n", &newOrder->totalCost);

        if (*root == NULL)
        {
            *root = newOrder;
        }
        else
        {
            OrderNode *current = *root;
            while (1)
            {
                if (newOrder->orderNumber < current->orderNumber)
                {
                    if (current->left == NULL)
                    {
                        current->left = newOrder;
                        break;
                    }
                    else
                    {
                        current = current->left;
                    }
                }
                else
                {
                    if (current->right == NULL)
                    {
                        current->right = newOrder;
                        break;
                    }
                    else
                    {
                        current = current->right;
                    }
                }
            }
        }
    }
}

// Function to free the memory allocated for orders
void freeOrders(OrderNode *root)
{
    if (root == NULL)
    {
        return;
    }
    freeOrders(root->left);
    freeOrders(root->right);

    free(root->menuItems);
    free(root);
}


int main()
{



    printf("\n");
    printf("\n\n\t\t\t\t\t**********ROYAL BLISS RESTAURANT**********\n\n\n\t\t");

    printf("\n\t\t\t\t\tthe project Represented by:\n");
    printf("\n\t\t\t\t\t61-Siddesh khaple\n");
    printf("\n\t\t\t\t\t63-Sujal khardekar\n");
    printf("\n\t\t\t\t\t64-Shruti khatal\n");
    printf("\n\t\t\t\t\t72-Ayushi kowe\n");


    MenuItem menu[] = {
        {0, "Misal",       50.0},
        {1, "Pav Bhaji",   75.0},
        {2, "Veg Noodles", 125.0},
        {3, "Shahi Paneer", 80.0},
        {4, "Cold coffee",  30.0},
        {5, "Mastani",      45.0},
        {6, "Hot chocolate",35.0},
        {7, "malai paneer", 130.0},
        {8, "dhokala",      60.0},
        {9, "Basundi",      70.0},
        {10,"chiken biryani",230.0}
        };

    int numItems = sizeof(menu) / sizeof(menu[0]);
    OrderNode *orders = NULL;

    int choice;
    do
    {
        
        printf("\n---- Welcome to our ROYAL BLISS RESTAURANT-----\n");
        printf("How can we help you..??\n");
        printf("1. Display Menu\n");
        printf("2. Place Order\n");
        printf("3. Display Order\n");
        printf("4. Delete Order\n");
        printf("5. Search Order\n");
        printf("6. Save Orders to File\n");
        printf("7. Load Orders from File\n");
        printf("8. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            displayMenu(menu, numItems);
            break;
        case 2:
        {
            int orderNumber;
            srand(time(NULL));
            orderNumber = rand() % 200 + 1;
            char customerName[50];
            // printf("Enter order number: ");
            // scanf("%d", &orderNumber);
            printf("order number  is %d ", orderNumber);
            printf("\nEnter customer name: ");
            scanf("%s", customerName);
            placeOrder(&orders, menu, numItems, orderNumber, customerName);
            break;
        }
        case 3:
        {
            int orderNumber;
            printf("Enter order number: ");
            scanf("%d", &orderNumber);
            searchOrder(orders, orderNumber);
            break;
        }
        case 4:
        {
            int orderNumber;
            printf("Enter order number: ");
            scanf("%d", &orderNumber);
            deleteOrder(&orders, orderNumber);
            break;
        }
        case 5:
        {
            int orderNumber;
            printf("Enter order number: ");
            scanf("%d", &orderNumber);
            searchOrder(orders, orderNumber);
            break;
        }
        case 6:
        {
            FILE *orderFile = fopen("orders.txt", "w");
            if (orderFile == NULL)
            {
                printf("Error: Could not open order file.\n");
                break;
            }
            saveOrdersToFile(orders, orderFile);
            fclose(orderFile);
            printf("Orders saved to file.\n");
            break;
        }
        case 7:
        {
            FILE *orderFile = fopen("orders.txt", "r");
            if (orderFile == NULL)
            {
                printf("Error: Could not open order file.\n");
                break;
            }
            loadOrdersFromFile(&orders, orderFile);
            fclose(orderFile);
            printf("Orders loaded from file.\n");
            break;
        }
        case 8:
            freeOrders(orders);
            printf("Exiting the program. Goodbye!\n");
            break;
        default:
            printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 8);
    return 0;
}