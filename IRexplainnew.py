IR_FILE="./a.ir"
ASM_FILE="./a.asm"

def do_func(start_location):
    global output_code
    output_code.append('    push ebp')
    output_code.append('    mov ebp,esp')
    j=6
    for i in args_name[code[start_location-1][6:]]:
        newline='    mov eax,[ebp+'+str(j)+']'
        output_code.append(newline)
        newline='    mov '+i+',eax'
        output_code.append(newline)
        j+=4
    
    for i in range(start_location,len(code)):
        if (code[i][0:4]=='push'):
            newline='    mov eax,'+code[i][5:]
            output_code.append(newline)
            output_code.append('    push eax')
        elif (code[i][0:3]=='pop'):
            output_code.append('    pop eax')
            newline='    mov '+code[i][4:]+',eax'
            output_code.append(newline)
        elif (code[i][0:3]=='cmp'):
            if (code[i][3:]=='a'):
                code[i]='cmpbe'
            elif (code[i][3:]=='b'):
                code[i]='cmpae'
            elif (code[i][3:]=='ae'):
                code[i]='cmpb'
            elif (code[i][3:]=='be'):
                code[i]='cmpa'
            elif (code[i][3:]=='e'):
                code[i]='cmpne'
            elif (code[i][3:]=='ne'):
                code[i]='cmpe'
        elif (code[i][0:2]=='jz'):
            output_code.append('    pop eax')
            output_code.append('    pop ebx')
            output_code.append('    cmp ebx,eax')
            newline='    j'+code[i-1][3:]+' '+code[i][3:]
            output_code.append(newline)
        elif (code[i][0:3]=='jmp'):
            newline='    '+code[i]
            output_code.append(newline)
        elif (code[i]=='add'):
            output_code.append('    pop ebx')
            output_code.append('    pop eax')
            output_code.append('    add eax,ebx')
            output_code.append('    push eax')
        elif (code[i]=='sub'):
            output_code.append('    pop ebx')
            output_code.append('    pop eax')
            output_code.append('    sub eax,ebx')
            output_code.append('    push eax')
        elif (code[i]=='mul'):
            output_code.append('    pop ebx')
            output_code.append('    pop eax')
            output_code.append('    imul eax,ebx')
            output_code.append('    push eax')
        elif (code[i]=='div'):
            output_code.append('    pop ebx')
            output_code.append('    pop eax')
            output_code.append('    xor edx,edx')
            output_code.append('    div ebx')
            output_code.append('    push eax')
        elif (code[i]=='and'):
            output_code.append('    pop eax')
            output_code.append('    pop ebx')
            output_code.append('    and eax,ebx')
            output_code.append('    push eax')
        elif (code[i]=='or'):
            output_code.append('    pop eax')
            output_code.append('    pop ebx')
            output_code.append('    or eax,ebx')
            output_code.append('    push eax')
        elif (code[i]=='xor'):
            output_code.append('    pop eax')
            output_code.append('    pop ebx')
            output_code.append('    xor eax,ebx')
            output_code.append('    push eax')
        elif (code[i][0:4]=='CALL'):
            newline='    call '+code[i][5:]
            output_code.append(newline)
            newline='    add esp,'+str(len(args_name[code[i][5:]])*4)
            output_code.append(newline)
            output_code.append('    push eax')
        elif (code[i]=='ret'):
            output_code.append('    pop eax')
            output_code.append('    pop ebp')
            output_code.append('    ret')
        elif (code[i]=='ENDFUNC'):
            output_code.append('')
            if (code[start_location-1]=='FUNC @main'):
                output_code.append('    mov eax,0')
                output_code.append('    pop ebp')
                output_code.append('    ret')
            break
        else:
            output_code.append(code[i])



with open(IR_FILE,'r',encoding = "utf-8") as ir:
    ircode=ir.readlines()
    
code=[]
for line in ircode:
    if (line[0]=='\t'):
        code.append(line[1:-1])
    else:
        code.append(line[:-1])
print(code)

output_code=['.386','data segment use16','    my_ret dd 0']
i=0
args_name={'main':[]}
for line in code:
    if (line[0:6]=='FUNC @'):
        func_name=line[6:]
    if (line[0:3]=='var' or line[0:4]=='args'):
        newline='    '+line[4:]+' dd 0'
        output_code.append(newline)
        code[i]=''
    if (line[0:4]=='args'):
        if func_name in args_name:
            args_name[func_name].append(line[5:])
        else:
            args_name[func_name]=[line[5:]]
    i+=1
print(args_name)
output_code.append('data ends')
output_code.append('')

output_code.append('code segment use16')
output_code.append('assume cs:code,ds:data')
output_code.append('')

for i in range(0,len(code)):
    if (code[i][0:6]=='FUNC @'):
        newline=code[i][6:]+':'
        output_code.append(newline)
        do_func(i+1)

output_code.append('my_IRexplain_main:')
output_code.append('    mov ax,data')
output_code.append('    mov ds,ax')
output_code.append('    call main')
output_code.append('    add eax,41h')
output_code.append('    mov dl,al')
output_code.append('    mov ah,02h')
output_code.append('    int 21h')
output_code.append('')
output_code.append('    mov ah,4Ch')
output_code.append('    int 21h')
output_code.append('')
output_code.append('code ends')
output_code.append('end my_IRexplain_main')

for i in output_code:
    print(i)

with open(ASM_FILE,'w',encoding = "utf-8") as asm:
    for i in output_code:
        asm.write(i)
        asm.write('\n')