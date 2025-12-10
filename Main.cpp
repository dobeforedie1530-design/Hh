#include <iostream>
#include <string>
#include <stack>
#include <queue>
#include <iomanip>
using namespace std;

// Transaction structure
struct Transaction {
    string type;
    double amount;
    double balanceBefore;
    double balanceAfter;
    
    Transaction(string t, double amt, double before, double after) 
        : type(t), amount(amt), balanceBefore(before), balanceAfter(after) {}
};

// BST Node for Account
struct AccountNode {
    int accountNumber;
    string accountHolder;
    double balance;
    AccountNode* left;
    AccountNode* right;
    
    AccountNode(int accNum, string holder, double bal) 
        : accountNumber(accNum), accountHolder(holder), balance(bal), left(nullptr), right(nullptr) {}
};

// Banking System Class
class BankingSystem {
private:
    AccountNode* root;
    stack<Transaction> transactionHistory;
    queue<Transaction> fraudQueue;
    double fraudThreshold;
    
    // Insert account into BST
    AccountNode* insertAccount(AccountNode* node, int accNum, string holder, double bal) {
        if (node == nullptr) {
            return new AccountNode(accNum, holder, bal);
        }
        
        if (accNum < node->accountNumber) {
            node->left = insertAccount(node->left, accNum, holder, bal);
        } else if (accNum > node->accountNumber) {
            node->right = insertAccount(node->right, accNum, holder, bal);
        } else {
            cout << "Account number already exists!\n";
        }
        
        return node;
    }
    
    // Search for account in BST
    AccountNode* searchAccount(AccountNode* node, int accNum) {
        if (node == nullptr || node->accountNumber == accNum) {
            return node;
        }
        
        if (accNum < node->accountNumber) {
            return searchAccount(node->left, accNum);
        }
        
        return searchAccount(node->right, accNum);
    }
    
    // In-order traversal to display all accounts
    void inorderTraversal(AccountNode* node) {
        if (node == nullptr) return;
        
        inorderTraversal(node->left);
        cout << "Account #: " << node->accountNumber 
             << " | Holder: " << node->accountHolder 
             << " | Balance: $" << fixed << setprecision(2) << node->balance << endl;
        inorderTraversal(node->right);
    }
    
public:
    BankingSystem() : root(nullptr), fraudThreshold(10000.0) {}
    
    // Add new account
    void addAccount(int accNum, string holder, double initialBalance) {
        root = insertAccount(root, accNum, holder, initialBalance);
        cout << "Account created successfully!\n";
    }
    
    // Deposit money
    void deposit(int accNum, double amount) {
        AccountNode* account = searchAccount(root, accNum);
        
        if (account == nullptr) {
            cout << "Account not found!\n";
            return;
        }
        
        if (amount <= 0) {
            cout << "Invalid deposit amount!\n";
            return;
        }
        
        double balanceBefore = account->balance;
        account->balance += amount;
        
        Transaction trans("DEPOSIT", amount, balanceBefore, account->balance);
        transactionHistory.push(trans);
        
        // Fraud detection
        if (amount > fraudThreshold) {
            fraudQueue.push(trans);
            cout << "*** ALERT: Large deposit detected! Transaction flagged for review. ***\n";
        }
        
        cout << "Deposit successful! New balance: $" << fixed << setprecision(2) 
             << account->balance << endl;
    }
    
    // Withdraw money
    void withdraw(int accNum, double amount) {
        AccountNode* account = searchAccount(root, accNum);
        
        if (account == nullptr) {
            cout << "Account not found!\n";
            return;
        }
        
        if (amount <= 0) {
            cout << "Invalid withdrawal amount!\n";
            return;
        }
        
        if (account->balance < amount) {
            cout << "Insufficient balance!\n";
            return;
        }
        
        double balanceBefore = account->balance;
        account->balance -= amount;
        
        Transaction trans("WITHDRAW", amount, balanceBefore, account->balance);
        transactionHistory.push(trans);
        
        // Fraud detection
        if (amount > fraudThreshold) {
            fraudQueue.push(trans);
            cout << "*** ALERT: Large withdrawal detected! Transaction flagged for review. ***\n";
        }
        
        cout << "Withdrawal successful! New balance: $" << fixed << setprecision(2) 
             << account->balance << endl;
    }
    
    // Undo last transaction
    void undoLastTransaction(int accNum) {
        if (transactionHistory.empty()) {
            cout << "No transactions to undo!\n";
            return;
        }
        
        AccountNode* account = searchAccount(root, accNum);
        
        if (account == nullptr) {
            cout << "Account not found!\n";
            return;
        }
        
        Transaction lastTrans = transactionHistory.top();
        transactionHistory.pop();
        
        // Reverse the transaction
        account->balance = lastTrans.balanceBefore;
        
        cout << "Transaction undone! " << lastTrans.type << " of $" 
             << fixed << setprecision(2) << lastTrans.amount << " reversed.\n";
        cout << "Balance restored to: $" << account->balance << endl;
    }
    
    // Display fraud queue
    void displayFraudQueue() {
        if (fraudQueue.empty()) {
            cout << "No suspicious transactions.\n";
            return;
        }
        
        cout << "\n=== FRAUD QUEUE (Transactions Requiring Review) ===\n";
        queue<Transaction> tempQueue = fraudQueue;
        int count = 1;
        
        while (!tempQueue.empty()) {
            Transaction trans = tempQueue.front();
            tempQueue.pop();
            
            cout << count++ << ". Type: " << trans.type 
                 << " | Amount: $" << fixed << setprecision(2) << trans.amount
                 << " | Before: $" << trans.balanceBefore
                 << " | After: $" << trans.balanceAfter << endl;
        }
    }
    
    // Display all accounts
    void displayAllAccounts() {
        if (root == nullptr) {
            cout << "No accounts in the system.\n";
            return;
        }
        
        cout << "\n=== ALL ACCOUNTS ===\n";
        inorderTraversal(root);
    }
    
    // Set fraud threshold
    void setFraudThreshold(double threshold) {
        fraudThreshold = threshold;
        cout << "Fraud detection threshold set to: $" << fixed << setprecision(2) 
             << fraudThreshold << endl;
    }
    
    // Check balance
    void checkBalance(int accNum) {
        AccountNode* account = searchAccount(root, accNum);
        
        if (account == nullptr) {
            cout << "Account not found!\n";
            return;
        }
        
        cout << "Account #: " << account->accountNumber << endl;
        cout << "Holder: " << account->accountHolder << endl;
        cout << "Balance: $" << fixed << setprecision(2) << account->balance << endl;
    }
};

// Main function with menu
int main() {
    BankingSystem bank;
    int choice, accNum;
    string holder;
    double amount;
    
    cout << "=== Banking Transaction Manager with Fraud Detection ===\n\n";
    
    while (true) {
        cout << "\n--- MENU ---\n";
        cout << "1. Add Account\n";
        cout << "2. Deposit\n";
        cout << "3. Withdraw\n";
        cout << "4. Check Balance\n";
        cout << "5. Undo Last Transaction\n";
        cout << "6. Display Fraud Queue\n";
        cout << "7. Display All Accounts\n";
        cout << "8. Set Fraud Threshold\n";
        cout << "9. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;
        
        switch (choice) {
            case 1:
                cout << "Enter account number: ";
                cin >> accNum;
                cout << "Enter account holder name: ";
                cin.ignore();
                getline(cin, holder);
                cout << "Enter initial balance: ";
                cin >> amount;
                bank.addAccount(accNum, holder, amount);
                break;
                
            case 2:
                cout << "Enter account number: ";
                cin >> accNum;
                cout << "Enter deposit amount: ";
                cin >> amount;
                bank.deposit(accNum, amount);
                break;
                
            case 3:
                cout << "Enter account number: ";
                cin >> accNum;
                cout << "Enter withdrawal amount: ";
                cin >> amount;
                bank.withdraw(accNum, amount);
                break;
                
            case 4:
                cout << "Enter account number: ";
                cin >> accNum;
                bank.checkBalance(accNum);
                break;
                
            case 5:
                cout << "Enter account number: ";
                cin >> accNum;
                bank.undoLastTransaction(accNum);
                break;
                
            case 6:
                bank.displayFraudQueue();
                break;
                
            case 7:
                bank.displayAllAccounts();
                break;
                
            case 8:
                cout << "Enter new fraud threshold: ";
                cin >> amount;
                bank.setFraudThreshold(amount);
                break;
                
            case 9:
                cout << "Thank you for using the Banking System!\n";
                return 0;
                
            default:
                cout << "Invalid choice! Please try again.\n";
        }
    }
    
    return 0;
}