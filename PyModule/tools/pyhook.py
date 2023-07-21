import pyautogui
import subprocess,time
recorded_actions = []
while True:
    action = pyautogui.prompt('请输入操作命令 (q表示退出录制):')
    if action == 'q':
        break
    recorded_actions.append(action)
with open('recorded_actions.txt', 'w') as file:
    for action in recorded_actions:
        file.write(action + '\n')
def simulate_actions():
    with open('recorded_actions.txt', 'r') as file:
        actions = file.readlines()
        
        for action in actions:
            action = action.strip()
            
            # 使用subprocess执行3D Slicer的命令行界面
            # subprocess.call(['Slicer', '--no-main-window', '--python-code', action])
            if action.startswith('move'):
                x, y = action.split()[1:]
                pyautogui.moveTo(int(x), int(y))
                
            elif action.startswith('click'):
                x, y = action.split()[1:]
                pyautogui.click(int(x), int(y))
                
            elif action.startswith('press'):
                key = action.split()[1]
                pyautogui.keyDown(key)
                
            elif action.startswith('release'):
                key = action.split()[1]
                pyautogui.keyUp(key)
                
            time.sleep(0.5)  # 可选的延迟时间
simulate_actions()
