import csv

class Trade:
    def __init__(self, save, name, ticker, side, filled, tot_qty, price, time_place, time_filled):
        self.save = save
        self.name = name
        self.ticker = ticker
        self.side = side  # -1 for buy, 1 for sell
        self.filled = filled
        self.tot_qty = tot_qty
        self.price = price
        self.time_place = time_place
        self.time_filled = time_filled

class NodeT:
    def __init__(self, trade):
        self.trade = trade
        self.next = None

class Stock:
    def __init__(self, ticker):
        self.ticker = ticker
        self.num_trades = 0
        self.pl = 0.0
        self.trades = None

class NodeS:
    def __init__(self, stock):
        self.stock = stock
        self.next = None

def import_trade(row):
    """Converts a CSV row into a Trade object."""
    if row[3] == "Cancelled":
        return Trade(0, "/0", "/0", 0, 0, 0, 0.0, "/0", "/0")
    if row[0] == "Name":
        return Trade(0, "/0", "/0", 0, 0, 0, 0.0, "/0", "/0")
    
    side = -1 if row[2] == "Buy" else 1
    return Trade(1, row[0], row[1], side, int(row[4]), int(row[5]), float(row[7]), row[9], row[10])

def find_stock(stock_list, ticker):
    """Finds a stock by ticker in the stock linked list."""
    trav = stock_list
    while trav:
        if trav.stock.ticker == ticker:
            return trav
        trav = trav.next
    return None

def trade_sort(tl_head, sl_head):
    """Sorts trades into their respective stocks in the stock linked list."""
    tl_trav = tl_head.next
    while tl_trav.trade:
        stock_node = find_stock(sl_head, tl_trav.trade.ticker)
        if not stock_node:
            # Create a new stock node if it doesn't exist
            new_stock = Stock(tl_trav.trade.ticker)
            new_stock_node = NodeS(new_stock)
            new_stock_node.stock.trades = NodeT(tl_trav.trade)
            # Append the new stock node to the stock linked list
            trav = sl_head
            while trav.next:
                trav = trav.next
            trav.next = new_stock_node
        else:
            # Add trade to existing stock
            stock_node.stock.num_trades += 1
            trade_node = stock_node.stock.trades
            while trade_node.next:
                trade_node = trade_node.next
            trade_node.next = NodeT(tl_trav.trade)
        tl_trav = tl_trav.next

def stock_calc_pl(stock_count, sl_head):
    """Calculates the profit/loss for each stock."""
    sl_trav = sl_head
    while sl_trav:
        trade_node = sl_trav.stock.trades
        while trade_node:
            sl_trav.stock.pl += trade_node.trade.side * trade_node.trade.price * trade_node.trade.filled
            trade_node = trade_node.next
        sl_trav = sl_trav.next

def print_menu():
    """Displays the menu options."""
    print("Trading Data Menu")
    print("Please select one below:")
    print("l: List all non-cancelled trades in data")
    print("s: List all stocks that were traded (Unordered)")
    print("d: Basic data about # of trades, stocks, and current P/L")
    print("x: Exit")

def main():
    FI = "Webull_Orders_Records.csv"
    
    # Read CSV file and create trade linked list
    with open(FI, "r") as file:
        reader = csv.reader(file)
        tl_head = NodeT(None)  # Start with an empty node
        tl_trav = tl_head
        total_pl = 0.0
        num_tot_trades = -1
        num_filled_trades = 0
        num_stocks_traded = 0
        sl_head = NodeS(Stock("Symb"))  # Initialize the stock linked list
        
        # Process each row in the CSV
        for row in reader:
            trade = import_trade(row)
            num_tot_trades += 1
            if trade.save == 1:
                tl_trav.trade = trade
                total_pl += trade.filled * trade.price * trade.side
                tl_trav.next = NodeT(None)
                tl_trav = tl_trav.next
                
        
        # Sort trades into stock linked list
        trade_sort(tl_head, sl_head)
        
        # Count the number of trades filled and stocks traded
        temp_t = tl_head.next
        while temp_t:
            num_filled_trades += 1
            temp_t = temp_t.next
        
        temp_s = sl_head.next
        while temp_s:
            num_stocks_traded += 1
            temp_s = temp_s.next
        
        # Calculate P/L for each stock
        stock_calc_pl(num_stocks_traded, sl_head)

        # Menu loop
        sel = ''
        while sel != 'x':
            print_menu()
            sel = input().strip().lower()
            if sel == 'l':
                temp1 = tl_head
                i = 0
                while temp1.trade:
                    if temp1.trade.side == 1:  # Sell
                        print(f"{i+1}: Sell {temp1.trade.ticker} {temp1.trade.filled}/{temp1.trade.tot_qty} Filled @ {temp1.trade.price:.2f} Placed: {temp1.trade.time_place} Filled: {temp1.trade.time_filled}")
                    else:  # Buy
                        print(f"{i+1}: Buy {temp1.trade.ticker} {temp1.trade.filled}/{temp1.trade.tot_qty} Filled @ {temp1.trade.price:.2f} Placed: {temp1.trade.time_place} Filled: {temp1.trade.time_filled}")
                    temp1 = temp1.next
                    i += 1
            elif sel == 's':
                temp2 = sl_head.next
                i = 0
                while temp2:
                    print(f"{i+1}: {temp2.stock.ticker} \n  # Trades: {temp2.stock.num_trades} \n  Stock P/L: ${temp2.stock.pl:.2f}")
                    temp2 = temp2.next
                    i += 1
            elif sel == 'd':
                print(f"# of Trade Orders Placed: {num_tot_trades}")
                print(f"# of Trades Filled: {num_filled_trades}")
                print(f"# of Stocks Traded: {num_stocks_traded}")
                print(f"Total P/L: ${total_pl:.2f}")
            elif sel == 'x':
                print("Goodbye, thank you for trading!")
            else:
                print("Invalid character entered, please select from the options.")

if __name__ == "__main__":
    main()
