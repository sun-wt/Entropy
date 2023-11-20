import os
import random
import numpy as np
from time import time
import tensorflow as tf
from copy import deepcopy
from itertools import chain
from collections import defaultdict

os.environ['TF_CPP_MIN_LOG_LEVEL'] = '2'
model_filename = 'mix90-97.h5'
model_path = os.path.join(os.getcwd(), model_filename)
model = tf.keras.models.load_model(model_path, custom_objects={})

class RP(object):
    def __init__(self, player, board=[]):
        if len(board) == 0:
            self.board = self.input_board()
        else:
            self.board = board
        self.color_base = self.fix_color_base()
        self.player = player
        
    def input_board(self):
        board = np.zeros((7, 7), dtype=int)
        for row in range(7):
            board[row] = input().split()
        print()
        return board
        
    def fix_color_base(self, board=[]):
        if len(board) == 0:
            board = self.board
        color_base = []
        # 降維
        dr = list(chain.from_iterable(board))
        for i in range(1, 8):
            color_base.append([i] * (7 - dr.count(i)))
        color_base = list(chain.from_iterable(color_base))
        random.shuffle(color_base)
        color_base = list(color_base)
        return color_base

class MCTS_Middle(object):
    def __init__(self, board, main_player, color_base, color=0, mode=0, score=0, simu_player=None, parent=None, parent_action=None):
        self.board = board
        self.main_player = main_player
        if simu_player == 'CHAOS':
            self.color = color if color != 0 else color_base.pop()
        elif simu_player == 'ORDER':
            self.color = 0
        else:
            self.color = color
        self.color_base = color_base 
        self.chess_num = mode 
        
        self.simu_player = simu_player       
        if self.simu_player == None:
            self.simu_player = main_player      
        self.parent = parent
        self.parent_action = parent_action
        
        self.visited_time = 0
        self.children = None
        self.children = []
        self.game_results = defaultdict(int)
        self.game_results[-1] = 0
        self.game_results[1] = 0
        self.unpassed_actions = None
        self.unpassed_actions = self.get_legal_actions()
        
        if score == 0:
            self.original_score = self.get_score()
        else:
            self.original_score = score
        
    # 取得合法動作 
    def get_legal_actions(self):
        # 現在是模擬玩家
        player = self.simu_player
        if player == 'CHAOS':
            return self.get_chaos_actions()
        elif player == 'ORDER':
            return self.get_order_actions()
        else:
            return None
            
    # chaos的合法下棋
    def get_chaos_actions(self):
        positions = np.where(self.board == 0)
        actions = list(zip(positions[0], positions[1]))
        return actions
    
    # order的合法下棋
    def get_order_actions(self):
        positions = np.where(self.board != 0)
        sources = list(zip(positions[0], positions[1]))
        actions = []
        
        for action in sources:
            row, col = action[0], action[1]
            row_pair = [[row - 1, -1, -1], [row + 1, 7, 1]]
            col_pair = [[col - 1, -1, -1], [col + 1, 7, 1]]
            for i in range(2):
                for j in range(row_pair[i][0], row_pair[i][1], row_pair[i][2]):
                    if self.board[j][col] != 0:
                        break
                    actions.append((action[0], action[1], j, col))
                for j in range(col_pair[i][0], col_pair[i][1], col_pair[i][2]):
                    if self.board[row][j] != 0:
                        break
                    actions.append((action[0], action[1], row, j))
        actions.append((sources[0][0], sources[0][1], sources[0][0], sources[0][1]))
        return actions
     
    # 勝場數-敗場數
    def q(self):
        return self.game_results[1] - self.game_results[-1]
    
    # 節點拜訪次數
    def n(self):
        return self.visited_time

    # 擴展
    def expand(self):
        cur = self
        if len(cur.unpassed_actions) == 0:
            return None
        action = cur.unpassed_actions.pop()
        moved_board = cur.move(color=self.color, source=action[:2], dest=action[2:])

        simu_player = 'CHAOS' if cur.simu_player == 'ORDER' else 'ORDER'            
        child_node = MCTS_Middle(board=moved_board, color_base=deepcopy(cur.color_base), mode=self.chess_num, score=self.original_score, main_player=self.main_player, simu_player=simu_player, parent=cur, parent_action=action)    
        cur.children.append(child_node)
        return child_node
    
    # 遊戲是否結束
    def is_game_over(self):
        return True if np.count_nonzero(self.board) > self.chess_num + 5 else False

    # 移動棋盤
    def move(self, source, dest=None, color=0):
        b = deepcopy(self.board)
        if self.simu_player == 'CHAOS':
            b[source[0]][source[1]] = color
            return b
        else:
            tmp = b[source[0]][source[1]]
            b[source[0]][source[1]] = b[dest[0]][dest[1]] 
            b[dest[0]][dest[1]] = tmp
            return b
        
    def rollout(self):
        cur = self
        while not cur.is_game_over():
            actions = cur.get_legal_actions()
            action = cur.rollout_rule(actions)        
            moved_board = cur.move(color=cur.color, source=action[:2], dest=action[2:])

            simu_player = 'CHAOS' if cur.simu_player == 'ORDER' else 'ORDER'   
            child_node = MCTS_Middle(board=moved_board, color_base=deepcopy(cur.color_base), mode=self.chess_num, score=self.original_score, main_player=self.main_player, simu_player=simu_player, parent=cur, parent_action=action)    
            cur.children.append(child_node)            
            cur = child_node
        return cur
             
    def rollout_rule(self, actions):
        return actions[np.random.randint(len(actions))]
    
    def get_score(self):
        # return run_nn(self.board)
        board = self.board.flatten()        
        score = 0
        for k in range(2, 8):
            for i in range(7):
                for j in range(8 - k):
                    idx  = i * 7 + j
                    if k <= 3:
                        if board[idx] == 0:
                            continue
                        if board[idx + 1 + (k % 2)] == board[idx]:
                            score += k
                    elif k <= 5:
                        if board[idx] == 0 or board[idx + 1] == 0:
                            continue
                        if board[idx + 2 + (k % 2)] == board[idx + 1] and board[idx + 3 + (k % 2)] == board[idx]:
                            score += k
                    else:
                        if board[idx] == 0 or board[idx + 1] == 0 or board[idx + 2] == 0:
                            continue
                        if board[idx + 3 + (k % 2)] == board[idx + 2] and board[idx + 4 + (k % 2)] == board[idx + 1] and board[idx + 5 + (k % 2)] == board[idx]:
                            score += k
          
            for j in range(7):
                for i in range(8 - k):
                    idx = i * 7 + j
                    if k <= 3:
                        if board[idx] == 0:
                            continue
                        if board[idx + 7 + (k % 2) * 7] == board[idx]:
                            score += k
                    elif k <= 5:
                        if board[idx] == 0 or board[idx + 7] == 0:
                            continue
                        if board[idx + 14 + (k % 2) * 7] == board[idx + 7] and board[idx + 21 + (k % 2) * 7] == board[idx]:
                            score += k
                    else:
                        if board[idx] == 0 or board[idx + 7] == 0 or board[idx + 14] == 0:
                            continue
                        if board[idx + 21 + (k % 2) * 7] == board[idx + 14] and board[idx + 28 + (k % 2) * 7] == board[idx + 7] and board[idx + 35 + (k % 2) * 7] == board[idx]:
                            score += k
        return score

    def game_result(self):   
        score = self.get_score()         
        if self.main_player == 'CHAOS':
            standard = -int(3 + 0.07 * self.chess_num)
            gap = self.original_score - score
            if gap == standard:
                return 0, 0
            elif gap < standard:
                return -1, abs(gap-standard)
            else:
                return 1, abs(gap-standard)
        else:
            standard = int(6 + 0.07 * self.chess_num)
            gap = score - self.original_score            
            if gap == standard:
                return 0, 0
            if gap < standard:
                return -1, abs(gap-standard)
            else:
                return 1, abs(gap-standard)
                        
    def backpropagate(self, result=None, point=None):
        self.visited_time += 1
        if point == None:
            result, point = self.game_result()
        self.game_results[result] += point
        if self.parent:
            self.parent.backpropagate(result, point)   
            
    def is_fully_expanded(self):
        return len(self.unpassed_actions) == 0
    
    def select(self, c=15):
        ucb_array = []
        for child in self.children:
            ucb_array.append((child.q() / child.n()) + c * np.sqrt((2 * np.log(self.n()) / child.n()))) 
        return self.children[np.argmax(ucb_array)]
    
    def opselect(self, c=15):
        ucb_array = []
        for child in self.children:
            ucb_array.append((-child.q() / child.n()) + c * np.sqrt((2 * np.log(self.n()) / child.n()))) 
        return self.children[np.argmax(ucb_array)]
    
    def tree_rule(self):
        cur = self
        while not cur.is_game_over():         
            if not cur.is_fully_expanded():
                return cur.expand()
            else:
                if cur.simu_player == cur.main_player:
                    cur = cur.select()
                else:
                    cur = cur.opselect()
        return cur
    
    def best_action(self, simulate_time = 10000): 
        node = None       
        for i in range(simulate_time):
            node = self.tree_rule()
            end = node.rollout()
            end.backpropagate()
            
        return self.select(c=0)
  
class Node(object):
    def __init__(self, player):
        self.board = np.zeros((7, 7), dtype=int)
        self.player = player 
     
    def get_score(self):
        return run_nn(self.board)
        board = self.board.flatten()        
        score = 0
        for k in range(2, 8):
            for i in range(7):
                for j in range(8 - k):
                    idx  = i * 7 + j
                    if k <= 3:
                        if board[idx] == 0:
                            continue
                        if board[idx + 1 + (k % 2)] == board[idx]:
                            score += k
                    elif k <= 5:
                        if board[idx] == 0 or board[idx + 1] == 0:
                            continue
                        if board[idx + 2 + (k % 2)] == board[idx + 1] and board[idx + 3 + (k % 2)] == board[idx]:
                            score += k
                    else:
                        if board[idx] == 0 or board[idx + 1] == 0 or board[idx + 2] == 0:
                            continue
                        if board[idx + 3 + (k % 2)] == board[idx + 2] and board[idx + 4 + (k % 2)] == board[idx + 1] and board[idx + 5 + (k % 2)] == board[idx]:
                            score += k
          
            for j in range(7):
                for i in range(8 - k):
                    idx = i * 7 + j
                    if k <= 3:
                        if board[idx] == 0:
                            continue
                        if board[idx + 7 + (k % 2) * 7] == board[idx]:
                            score += k
                    elif k <= 5:
                        if board[idx] == 0 or board[idx + 7] == 0:
                            continue
                        if board[idx + 14 + (k % 2) * 7] == board[idx + 7] and board[idx + 21 + (k % 2) * 7] == board[idx]:
                            score += k
                    else:
                        if board[idx] == 0 or board[idx + 7] == 0 or board[idx + 14] == 0:
                            continue
                        if board[idx + 21 + (k % 2) * 7] == board[idx + 14] and board[idx + 28 + (k % 2) * 7] == board[idx + 7] and board[idx + 35 + (k % 2) * 7] == board[idx]:
                            score += k
        return score

    def greedy_chaos(self, color):
        board = self.board
        pick_board = np.zeros((7, 7), dtype=int)
        score, original_score = 0, self.get_score()
    
        for i in range(7):
            for j in range(7):
                if board[i][j] != 0:
                    pick_board[i][j] = 1000
           
        for i in range(7):
            for j in range(7):
                if board[i][j] == 0:
                    board[i][j] = color
                    score = self.get_score()
                    board[i][j] = 0
                    if score > original_score:
                        k = 0
                        while i >= k and pick_board[i - k][j] != 1000:
                            pick_board[i - k][j] += 1
                            k += 1
                        
                        k = 0
                        while i + k < 7 and pick_board[i + k][j] != 1000:
                            pick_board[i + k][j] += 1
                            k += 1
                        
                        k = 0
                        while j >= k and pick_board[i][j - k] != 1000:
                            pick_board[i][j - k] += 1
                            k += 1
                       
                        k = 0
                        while j + k < 7 and pick_board[i][j + k] != 1000:
                            pick_board[i][j + k] += 1
                            k += 1
                       
        find_min, min_row, min_col = 10000, 0, 0
        
        for i in range(2, 5):
            for j in range(2, 5):
                if pick_board[i][j] < find_min:
                    find_min, min_row, min_col = pick_board[i][j], i, j 
            for j in range(2):
                if pick_board[i][j] < find_min:
                    find_min, min_row, min_col = pick_board[i][j], i, j
            for j in range(5, 7):
                if pick_board[i][j] < find_min:
                    find_min, min_row, min_col = pick_board[i][j], i, j

        for i in range(2):
            for j in range(7):
                if pick_board[i][j] < find_min:
                    find_min, min_row, min_col = pick_board[i][j], i, j
                    
        for i in range(5, 7):
            for j in range(7):
                if pick_board[i][j] < find_min:
                    find_min, min_row, min_col = pick_board[i][j], i, j
        
        return min_row, min_col
          
    def advanced_order(self):
        board = self.board        
        max_score = self.get_score()
        action = [-1, -1, -1, -1]
        still = [0, 0, 0, 0]
        
        for row in range(7):
            for col in range(7):
                if board[row][col] != 0:
                    still = [row, col, row, col]
                    # right
                    for i in range(1, 7-col):
                        if self.board[row][col + i] == 0:
                            self.move([row, col], [row, col+i])
                            score = self.get_score() + 0.2 * np.abs(i-3)
                            same = np.count_nonzero(board[:, col + i] == board[row, col + i]) - 1
                            score += same * 0.7
                            if score > max_score:
                                max_score = score
                                action = [row, col, row, col+i]
                            self.move([row, col+i], [row, col])
                        else: break
                    # left
                    for i in range(1, col+1):
                        if self.board[row][col - i] == 0:
                            self.move([row, col], [row, col-i])
                            score = self.get_score() + 0.2 * np.abs(i-3)
                            same = np.count_nonzero(board[:, col - i] == board[row, col-i]) - 1
                            score += same * 0.7
                            if score > max_score:
                                max_score = score
                                action = [row, col, row, col-i]
                            self.move([row, col-i], [row, col])
                        else: break
                    # up
                    for i in range(1, row+1):
                        if self.board[row - i][col] == 0:
                            self.move([row, col], [row-i, col])
                            score = self.get_score() + 0.2 * abs(i-3)
                            same = np.count_nonzero(board[row - i, :] == board[row - i, col]) - 1
                            score += same * 0.7
                            if score > max_score:
                                max_score = score
                                action = [row, col, row-i, col]
                            self.move([row-i, col], [row, col])
                        else: break
                    # down
                    for i in range(1, 7-row):
                        if self.board[row + i][col] == 0:
                            self.move([row, col], [row+i, col])
                            score = self.get_score() + 0.2 * abs(i-3)
                            same = np.count_nonzero(board[row + i, :] == board[row + i, col]) - 1
                            score += same * 0.7
                            if score > max_score:
                                max_score = score
                                action = [row, col, row+i, col]
                            self.move([row+i, col], [row, col])
                        else: break   
        if action[0] == -1:
            return still                 
        return action
     
    def move(self, source, dest=None, color=0):
        b = self.board
        if dest is None:
            b[source[0]][source[1]] = color
            return b
        else:
            tmp = b[source[0]][source[1]]
            b[source[0]][source[1]] = b[dest[0]][dest[1]] 
            b[dest[0]][dest[1]] = tmp
            return b
      
class Format():    
    def chaos_type(input):
        return int(input[0]), (ord(input[1])-65, ord(input[2])-97)
    
    def order_type(input):
        return (ord(input[0])-65, ord(input[1])-97, ord(input[2])-65, ord(input[3])-97)
    
    def print_chaos_action(action):
        print(chr(action[0]+65)+chr(action[1]+97))
        return
    
    def print_order_action(action):
        print(chr(action[0]+65)+chr(action[1]+97)+chr(action[2]+65)+chr(action[3]+97))
        return   

def run_nn(data):
    x_test = np.zeros((1, 147))  
    color_list = np.array([0, 0, 0, 0, 0, 0, 0])
    for i in range(7):
        for j in range(7):
            if data[i, j]==1:
                color_list[0]+=1
            elif data[i, j]==2:
                color_list[1]+=1
            elif data[i, j]==3:
                color_list[2]+=1
            elif data[i, j]==4:
                color_list[3]+=1
            elif data[i, j]==5:
                color_list[4]+=1
            elif data[i, j]==6:
                color_list[5]+=1
            elif data[i, j]==7:
                color_list[6]+=1
    for i in range(7):
        color_list[i] = 7-color_list[i]

    for i in range(7):
        for j in range(7):
            num = -1
            space = 0
            for m in range(7):
                if data[i, m] == data[i, j]:
                    num+=1
                if data[i, m] == 0:
                    space+=1
                if data[m, j] == data[i, j]:
                    num+=1
                if data[m, j] == 0:
                    space+=1
            # 目前盤面同色個數
            x_test[0, i*21+j*3] = num
            
            if data[i, j]!=0:
                # 不是空格
                x_test[0, i*21+j*3+1] = space
                # 同色個數
                x_test[0, i*21+j*3+2] = color_list[int(data[i, j])-1]
            else:
                #是空格
                x_test[0, i*21+j*3+1] = num
                x_test[0, i*21+j*3+2] = 0
    test_predictions = model.predict(x_test, verbose = 0).flatten()
    return np.round(test_predictions)

def main():
    Input = input()
    root = Node(player='CHAOS')
    chess_num, mcts_start, mcts_end = 0, 10, 40
    
    if Input != 'Start':
        root.player = 'ORDER'        
        while Input != 'Quit':
            color, opponent_action = Format.chaos_type(input=Input)
            root.move(source=opponent_action, color=color)
            chess_num += 1
            
            action = []
            if (mcts_start <= chess_num <= mcts_end) and chess_num % 4 == 0:
                action = mcts_rule(player=root.player, chess_num=chess_num, board=root.board, simulate_time=2850)  
            else:
                action = root.advanced_order()
            root.move(source=action[:2], dest=action[2:])
            
            Format.print_order_action(action)
            Input = input()
    else:
        while 1:
            Input = input()
            action = []
            if (mcts_start <= chess_num <= mcts_end) and chess_num % 4 == 0:
                action = mcts_rule(player=root.player, board=root.board, chess_num=chess_num, color=int(Input), simulate_time=3000)
            else:
                action = root.greedy_chaos(color=int(Input))
            root.move(source=action, color=int(Input))
            Format.print_chaos_action(action)
            chess_num += 1
            
            Input = input()   
            if Input == 'Quit': break         
            opponent_action = Format.order_type(input=Input)
            root.move(source=opponent_action[:2], dest=opponent_action[2:])
            
def mcts_rule(player, board, chess_num, color=0, simulate_time=1000):
    game = RP(player=player, board=board)
    root = MCTS_Middle(board=game.board, main_player=player, color_base=game.color_base, mode=chess_num, color=color)
    selected_node = root.best_action(simulate_time=simulate_time)
    return selected_node.parent_action    
main()