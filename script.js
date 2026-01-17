document.addEventListener('DOMContentLoaded', function() {
    const themeToggle = document.getElementById('themeToggle');
    const currentTheme = localStorage.getItem('theme') || 'light';
    
    document.documentElement.setAttribute('data-theme', currentTheme);
    if (themeToggle) {
        themeToggle.checked = currentTheme === 'dark';
    }

    if (themeToggle) {
        themeToggle.addEventListener('change', function() {
            const theme = this.checked ? 'dark' : 'light';
            document.documentElement.setAttribute('data-theme', theme);
            localStorage.setItem('theme', theme);
        });
    }
    
    const sortMembersBtn = document.getElementById('sortMembersBtn');
    
    if(sortMembersBtn) {
        sortMembersBtn.addEventListener('click', function() {
            const attr = document.getElementById("memberSortAttribute").value.trim();
            if(!attr){
                alert("Please choose a sort attribute");
                return;
            }

            const originalText = this.innerHTML;
            this.innerHTML = '<span class="btn-icon">⏳</span>Sorting...';
            this.disabled = true;

            fetch(`/sort-members?attribute=${encodeURIComponent(attr)}`)
                .then(res => {
                    if(!res.ok) throw new Error(`HTTP error! Status: ${res.status}`);
                    return res.text();
                })
                .then(() => {
                    setTimeout(() => {
                        const container = document.getElementById('memberList');
                        container.innerHTML = '';
                        membersVisible = false;
                        displayMembers();
                        
                        this.innerHTML = originalText;
                        this.disabled = false;
                    }, 500);
                })
                .catch(err => {
                    alert("Error sorting members: " + err.message);
                    console.error(err);
                    this.innerHTML = originalText;
                    this.disabled = false;
                });
        });
    }
});

function createBookCard(id, title, author, avail, borrowed) {
    const borrowedNum = parseInt(borrowed) || 0;
    const availNum = parseInt(avail) || 0;
    const total = borrowedNum + availNum;
    
    return `
        <div class="book-card">
            <div class="card-header">
                <h3>${title}</h3>
                <div class="status-badge">${availNum > 0 ? 'Available' : 'Out of Stock'}</div>
            </div>
            <div class="card-content">
                <div class="info-item">
                    <strong>ID:</strong> ${id}
                </div>
                <div class="info-item">
                    <strong>Author:</strong> ${author}
                </div>
                <div class="info-item">
                    <strong>Available:</strong> ${availNum} / ${total}
                </div>
                <div class="info-item">
                    <strong>Borrowed:</strong> ${borrowedNum}
                </div>
            </div>
            <div class="card-actions">
                <button onclick="deleteBook('${id}',1)" class="btn delete-btn">
                    Delete 1 Copy
                </button>
                <button onclick="deleteBook('${id}',0)" class="btn delete-btn">
                    Delete All Copies
                </button>
            </div>
        </div>
    `;
}

function createMemberCard(id, name, contact, borrowedBook) {
    let hasBorrowed = false;
    let borrowedText = 'None';
    
    if (borrowedBook) {
        const trimmed = borrowedBook.trim();
        if (trimmed !== '' && 
            trimmed !== 'None' && 
            trimmed !== 'null' && 
            trimmed !== 'undefined' &&
            !trimmed.toLowerCase().includes('none')) {
            hasBorrowed = true;
            borrowedText = trimmed;
        }
    }
    
    const status = hasBorrowed ? 'Borrowing' : 'Active';
    const statusClass = hasBorrowed ? 'status-badge warning' : 'status-badge';
    
    return `
        <div class="member-card">
            <div class="card-header">
                <h3>${name}</h3>
                <div class="${statusClass}">${status}</div>
            </div>
            <div class="card-content">
                <div class="info-item">
                    <strong>ID:</strong> ${id}
                </div>
                <div class="info-item">
                    <strong>Contact:</strong> ${contact}
                </div>
                <div class="info-item">
                    <strong>Borrowed Book:</strong> ${borrowedText}
                </div>
            </div>
            <div class="card-actions">
                <button onclick="deleteMember('${id}')" class="btn delete-btn">
                    Delete Member
                </button>
            </div>
        </div>
    `;
}

async function addBook(){
    const id=document.getElementById('bookId').value.trim();
    const title=document.getElementById('bookTitle').value.trim();
    const author=document.getElementById('bookAuthor').value.trim();
    const quantity=parseInt(document.getElementById('bookQuantity').value.trim());
    if(!id||!title||!author||isNaN(quantity)||quantity<=0) return alert("Enter valid info");
    
    try {
        const res=await fetch('/addBook',{
            method:'POST',
            headers:{'Content-Type':'application/json'},
            body:JSON.stringify({id,title,author,quantity})
        });
        if(!res.ok) throw new Error(`HTTP error! Status: ${res.status}`);
        alert(await res.text()); 
        displayBooks();
    } catch(err) {
        alert("Error adding book: " + err.message);
        console.error(err);
    }
}

let booksVisible = false;

async function displayBooks(){
    const container = document.getElementById('bookList');

    if(booksVisible){
        container.innerHTML = '';
        booksVisible = false;
        return;
    }

    try {
        const res = await fetch('/displayBooks');
        if(!res.ok) throw new Error(`HTTP error! Status: ${res.status}`);
        const text = await res.text();
        container.innerHTML = '';
        const lines = text.split('\n').filter(l => l.trim() !== '');
        
        if(lines.length === 0){
            container.innerHTML = '<div class="empty-state">✨ No books available</div>';
            booksVisible = true;
            return;
        }

        lines.forEach(line => {
            const [id, title, author, avail, borrowed] = line.split(',');
            container.innerHTML += createBookCard(id, title, author, avail, borrowed);
        });

        booksVisible = true;
    } catch(err) {
        alert("Error displaying books: " + err.message);
        console.error(err);
    }
}

async function countBooks(){
    try {
        const res=await fetch('/countBooks'); 
        if(!res.ok) throw new Error(`HTTP error! Status: ${res.status}`);
        const total=(await res.text()).trim();
        alert('Total books: '+total);
    } catch(err) {
        alert("Error counting books: " + err.message);
        console.error(err);
    }
}

async function deleteBook(id,quantity){
    const action = quantity === 0 ? 'ALL copies' : '1 copy';
    if(!confirm(`Are you sure you want to delete ${action} of book ${id}?`)) return;
    
    try {
        const res=await fetch('/deleteBook',{
            method:'POST',
            headers:{'Content-Type':'application/json'},
            body:JSON.stringify({id,quantity})
        });
        if(!res.ok) throw new Error(`HTTP error! Status: ${res.status}`);
        alert(await res.text()); 
        displayBooks();
    } catch(err) {
        alert("Error deleting book: " + err.message);
        console.error(err);
    }
}

async function addMember(){
    const id=document.getElementById('memberId').value.trim();
    const name=document.getElementById('memberName').value.trim();
    const contact=document.getElementById('memberContact').value.trim();
    if(!id||!name||!contact) return alert("Enter valid member info");
    
    try {
        const res=await fetch('/addMember',{
            method:'POST',
            headers:{'Content-Type':'application/json'},
            body:JSON.stringify({id,name,contact})
        });
        if(!res.ok) throw new Error(`HTTP error! Status: ${res.status}`);
        alert(await res.text()); 
        displayMembers();
    } catch(err) {
        alert("Error adding member: " + err.message);
        console.error(err);
    }
}

async function deleteMember(id){
    if(!confirm(`Are you sure you want to delete member ${id}?`)) return;
    
    try {
        const res=await fetch('/deleteMember',{
            method:'POST',
            headers:{'Content-Type':'application/json'},
            body:JSON.stringify({id})
        });
        if(!res.ok) throw new Error(`HTTP error! Status: ${res.status}`);
        alert(await res.text()); 
        displayMembers();
    } catch(err) {
        alert("Error deleting member: " + err.message);
        console.error(err);
    }
}

let membersVisible = false;

async function displayMembers(){
    const container = document.getElementById('memberList');

    if(membersVisible){
        container.innerHTML = '';
        membersVisible = false;
        return;
    }

    try {
        const res = await fetch('/displayMembers');
        if(!res.ok) throw new Error(`HTTP error! Status: ${res.status}`);
        const text = await res.text();
        container.innerHTML = '';
        const lines = text.split('\n').filter(l => l.trim() !== '');
        
        if(lines.length === 0){
            container.innerHTML = '<div class="empty-state">✨ No members available</div>';
            membersVisible = true;
            return;
        }

        lines.forEach(line => {
            const [id, name, contact, borrowedBook] = line.split(',');
            container.innerHTML += createMemberCard(id, name, contact, borrowedBook);
        });

        membersVisible = true;
    } catch(err) {
        alert("Error displaying members: " + err.message);
        console.error(err);
    }
}

async function borrowBook(){
    const memberId=document.getElementById('borrowMemberId').value.trim();
    const bookId=document.getElementById('borrowBookId').value.trim();
    if(!memberId||!bookId) return alert("Enter member ID and book ID");
    
    try {
        const res=await fetch('/borrowBook',{
            method:'POST',
            headers:{'Content-Type':'application/json'},
            body:JSON.stringify({memberId,bookId})
        });
        if(!res.ok) throw new Error(`HTTP error! Status: ${res.status}`);
        alert(await res.text()); 
        displayBooks(); 
        displayMembers();
    } catch(err) {
        alert("Error borrowing book: " + err.message);
        console.error(err);
    }
}

async function returnBook(){
    const memberId=document.getElementById('borrowMemberId').value.trim();
    const bookId=document.getElementById('borrowBookId').value.trim();
    if(!memberId||!bookId) return alert("Enter member ID and book ID");
    
    try {
        const res=await fetch('/returnBook',{
            method:'POST',
            headers:{'Content-Type':'application/json'},
            body:JSON.stringify({memberId,bookId})
        });
        if(!res.ok) throw new Error(`HTTP error! Status: ${res.status}`);
        alert(await res.text()); 
        displayBooks(); 
        displayMembers();
    } catch(err) {
        alert("Error returning book: " + err.message);
        console.error(err);
    }
}

async function searchBooks(){
    const keyword=document.getElementById('searchBookKeyword').value.trim();
    if(!keyword) return alert("Enter keyword");
    
    try {
        const res=await fetch('/searchBooks/'+encodeURIComponent(keyword));
        if(!res.ok) throw new Error(`HTTP error! Status: ${res.status}`);
        const text=await res.text();
        const container=document.getElementById('bookList'); 
        container.innerHTML='';
        const lines=text.split('\n').filter(l=>l.trim()!='');
        
        if(lines.length===0){ 
            container.innerHTML='<div class="empty-state">🔍 No matching books found</div>'; 
            return; 
        }
        
        lines.forEach(line=>{
            const [id, title, author, avail, borrowed] = line.split(',');
            container.innerHTML += createBookCard(id, title, author, avail, borrowed);
        });
    } catch(err) {
        alert("Error searching books: " + err.message);
        console.error(err);
    }
}

async function searchMembers(){
    const keyword=document.getElementById('searchMemberKeyword').value.trim();
    if(!keyword) return alert("Enter keyword");
    
    try {
        const res=await fetch('/searchMembers/'+encodeURIComponent(keyword)); 
        if(!res.ok) throw new Error(`HTTP error! Status: ${res.status}`);
        const text=await res.text();
        const container=document.getElementById('memberList'); 
        container.innerHTML='';
        const lines=text.split('\n').filter(l=>l.trim()!='');
        
        if(lines.length===0){ 
            container.innerHTML='<div class="empty-state">🔍 No matching members found</div>'; 
            return; 
        }
        
        lines.forEach(line=>{
            const [id, name, contact, borrowedBook] = line.split(',');
            container.innerHTML += createMemberCard(id, name, contact, borrowedBook);
        });
    } catch(err) {
        alert("Error searching members: " + err.message);
        console.error(err);
    }
}

function sortBooks(){
    const criterion = document.getElementById("sortCriterion").value.trim();
    if(!criterion){
        alert("Please choose a sort criterion");
        return;
    }

    const btn = document.querySelector('button[onclick="sortBooks()"]');
    const originalText = btn.innerHTML;
    btn.innerHTML = '<span class="btn-icon">⏳</span>Sorting...';
    btn.disabled = true;

    fetch(`/sort-books?attribute=${encodeURIComponent(criterion)}`)
        .then(res => {
            if(!res.ok) throw new Error(`HTTP error! Status: ${res.status}`);
            return res.text();
        })
        .then(() => {
            setTimeout(() => {
                const container = document.getElementById('bookList');
                container.innerHTML = '';
                booksVisible = false;
                displayBooks();
                
                btn.innerHTML = originalText;
                btn.disabled = false;
            }, 500);
        })
        .catch(err => {
            alert("Error sorting books: " + err.message);
            console.error(err);
            btn.innerHTML = originalText;
            btn.disabled = false;
        });
}