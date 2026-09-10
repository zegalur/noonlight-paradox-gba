vim.api.nvim_create_user_command("BuildGBA", function() 
    vim.cmd(':LspStop')
    vim.cmd('new')
    vim.cmd('terminal build-clang.bat')
    vim.cmd(':LspStart')
end, {})

vim.keymap.set("n", "<F2>", ":BuildGBA<CR>", { desc = "Builds this project for semantic syntax highlight." })
