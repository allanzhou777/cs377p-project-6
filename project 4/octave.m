
N = 11;
d = 0.85;
A = sparse(N, N);


edges = [
    2 3;
    3 2;
    4 1;
    4 2;
    5 2;
    5 4;
    5 6;
    6 2;
    6 5;
    7 2;
    7 5;
    8 2;
    8 5;
    9 2;
    9 5;
    10 5;
    11 5
];

for k = 1:size(edges, 1)
    i = edges(k, 1);
    j = edges(k, 2);
    A(j, i) = 1;
end

% outdegree
out_deg = sum(A, 1);


M = sparse(N, N);
for i = 1:N
    if out_deg(i) > 0
        M(:, i) = A(:, i) / out_deg(i);
    % for dangling nodes
    else
        M(:, i) = 1 / N; 
    end
end


G = d * M + (1 - d) / N * ones(N, N);

% initially PR = 1/N 
pagerank = ones(N, 1) / N;  
tolerance = 1e-6;
delta = 1;

while delta > tolerance
    new_pagerank = G * pagerank;
    delta = norm(new_pagerank - pagerank);
    pagerank = new_pagerank;
end

% normalize
pagerank = pagerank / sum(pagerank);


disp('PageRanks:');
disp(pagerank);
