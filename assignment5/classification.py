class Node:
    left = None
    right = None
    next_trie = None

    def __str__(self):
        return "(Left:%s Right:%s Next:%s)" % (str(self.left), str(self.right), str(self.next_trie))

root = Node()

class Rule:
    def __init__(self, number):
        self.number = number

    def __str__(self):
        return "Rule number:%d" % self.number

def addRule(rule, t1, t2):
    node = root
    new_next_trie = root.next_trie
    for i in t1:
        if i == '0':
            if node.left is None:
                node.left = Node()
            node = root.left
        elif i == '1':
            if node.right is None:
                node.right = Node()
            node = root.right
        # new_next_trie = mergeTries(new_next_trie, node.next_trie)
    # node.next_trie = new_next_trie
    if node.next_trie is None:
       node.next_trie = Node() 
    for i in t2:
        if i == '0':
            if node.left is None:
                node.left = Node()
            node = root.left
        elif i == '1':
            if node.right is None:
                node.right = Node()
            node = root.right
    if node.next_trie is None:
        node.next_trie = Rule(rule)

def classify(t1, t2):
    possible_rules = []
    node = root
    for i in t1:
        if i == '0':
            if node.left is None:
                break
            node = root.left
        elif i == '1':
            if node.right is None:
                break
            node = root.right
    node = node.next_trie
    for i in t2:
        if node.next_trie is not None:
            possible_rules.append(node.next_trie)
        if i == '0':
            if node.left is None:
                break
            node = root.left
        elif i == '1':
            if node.right is None:
                break
            node = root.right
    return possible_rules

for index, line in enumerate(open("rules.txt")):
    t1, l1, t2, l2 = line.split(" ")
    bin_t1 =  [ str(bin(int(i)))[2:] for i in t1.split(".") ]
    bin_t2 =  [ str(bin(int(i)))[2:] for i in t2.split(".") ]
    addRule(index+1, bin_t1, bin_t2)

with open("output.txt", 'w') as O:
    for index, line in enumerate(open("input")):
        t1, t2 = line.strip().split(" ")
        bin_t1 =  [ str(bin(int(i)))[2:] for i in t1.split(".") ]
        bin_t2 =  [ str(bin(int(i)))[2:] for i in t2.split(".") ]
        possible_rules = classify(bin_t1, bin_t2)
        O.write('%s %s %d %d\n' % (t1, t2, len(possible_rules), min(possible_rules+[Rule(-1)]).number))